// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2018 ISIS Rutherford Appleton Laboratory UKRI,
//   NScD Oak Ridge National Laboratory, European Spallation Source,
//   Institut Laue - Langevin & CSNS, Institute of High Energy Physics, CAS
// SPDX - License - Identifier: GPL - 3.0 +
#include "MantidDataHandling/SetSample.h"

#include "MantidAPI/MatrixWorkspace.h"
#include "MantidAPI/Run.h"
#include "MantidAPI/Sample.h"
#include "MantidDataHandling/CreateSampleShape.h"
#include "MantidDataHandling/SampleEnvironmentFactory.h"
#include "MantidGeometry/Instrument.h"
#include "MantidGeometry/Instrument/Container.h"
#include "MantidGeometry/Instrument/Goniometer.h"
#include "MantidGeometry/Instrument/ReferenceFrame.h"
#include "MantidGeometry/Instrument/SampleEnvironment.h"
#include "MantidGeometry/Objects/MeshObject.h"
#include "MantidGeometry/Objects/ShapeFactory.h"
#include "MantidKernel/ConfigService.h"
#include "MantidKernel/FacilityInfo.h"
#include "MantidKernel/InstrumentInfo.h"
#include "MantidKernel/Logger.h"
#include "MantidKernel/Material.h"
#include "MantidKernel/MaterialBuilder.h"
#include "MantidKernel/Matrix.h"
#include "MantidKernel/PropertyManager.h"
#include "MantidKernel/PropertyManagerProperty.h"

#include <Poco/Path.h>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/predicate.hpp>

namespace Mantid {
namespace DataHandling {

using API::ExperimentInfo;
using API::Workspace_sptr;
using Geometry::Container;
using Geometry::Goniometer;
using Geometry::ReferenceFrame;
using Geometry::SampleEnvironment;
using Geometry::ShapeFactory;
using Kernel::Logger;
using Kernel::MaterialBuilder;
using Kernel::PropertyManager;
using Kernel::PropertyManager_const_sptr;
using Kernel::PropertyWithValue;
using Kernel::V3D;

namespace {
constexpr double CUBIC_METRE_TO_CM = 100. * 100. * 100.;
constexpr double degToRad(const double x) { return x * M_PI / 180.; }

/// Private namespace storing property name strings
namespace PropertyNames {
/// Input workspace property name
const std::string INPUT_WORKSPACE("InputWorkspace");
/// Geometry property name
const std::string GEOMETRY("Geometry");
/// Material property name
const std::string MATERIAL("Material");
/// Environment property name
const std::string ENVIRONMENT("Environment");
/// Container geometry property name
const std::string CONTAINER_GEOMETRY("ContainerGeometry");
/// Container material property name
const std::string CONTAINER_MATERIAL("ContainerMaterial");
} // namespace PropertyNames
/// Private namespace storing sample environment args
namespace SEArgs {
/// Static Name string
const std::string NAME("Name");
/// Static Container string
const std::string CONTAINER("Container");
} // namespace SEArgs
/// Provate namespace storing geometry args
namespace GeometryArgs {
/// Static Shape string
const std::string SHAPE("Shape");
/// Static Value string for CSG
const std::string VALUE("Value");
} // namespace GeometryArgs

/// Private namespace storing sample environment args
namespace ShapeArgs {
/// Static FlatPlate string
const std::string FLAT_PLATE("FlatPlate");
/// Static Cylinder string
const std::string CYLINDER("Cylinder");
/// Static HollowCylinder string
const std::string HOLLOW_CYLINDER("HollowCylinder");
/// Static FlatPlateHolder string
const std::string FLAT_PLATE_HOLDER("FlatPlateHolder");
/// Static HollowCylinderHolder string
const std::string HOLLOW_CYLINDER_HOLDER("HollowCylinderHolder");
/// Static CSG string
const std::string CSG("CSG");
/// Static Width string
const std::string WIDTH("Width");
/// Static Height string
const std::string HEIGHT("Height");
/// Static Thick string
const std::string THICK("Thick");
/// Static FrontThick string
const std::string FRONT_THICK("FrontThick");
/// Static BackThick string
const std::string BACK_THICK("BackThick");
/// Static Axis string
const std::string AXIS("Axis");
/// Static Angle string
const std::string ANGLE("Angle");
/// Static Center string
const std::string CENTER("Center");
/// Static Radius string
const std::string RADIUS("Radius");
/// Static InnerRadius string
const std::string INNER_RADIUS("InnerRadius");
/// Static OuterRadius string
const std::string OUTER_RADIUS("OuterRadius");
/// Static InnerOuterRadius string
const std::string INNER_OUTER_RADIUS("InnerOuterRadius");
/// Static OuterInnerRadius string
const std::string OUTER_INNER_RADIUS("OuterInnerRadius");
} // namespace ShapeArgs

/**
 * Return the centre coordinates of the base of a cylinder given the
 * coordinates of the centre of the cylinder
 * @param cylCentre Coordinates of centre of the cylinder (X,Y,Z) (in metres)
 * @param height Height of the cylinder (in metres)
 * @param axis The index of the height-axis of the cylinder
 */
V3D cylBaseCentre(const std::vector<double> &cylCentre, double height,
                  unsigned axisIdx) {
  const V3D halfHeight = [&]() {
    switch (axisIdx) {
    case 0:
      return V3D(0.5 * height, 0, 0);
    case 1:
      return V3D(0, 0.5 * height, 0);
    case 2:
      return V3D(0, 0, 0.5 * height);
    default:
      return V3D();
    }
  }();
  return V3D(cylCentre[0], cylCentre[1], cylCentre[2]) - halfHeight;
}

/**
 * Return the centre coordinates of the base of a cylinder given the
 * coordinates of the centre of the cylinder
 * @param cylCentre Coordinates of centre of the cylinder (X,Y,Z) (in metres)
 * @param height Height of the cylinder (in metres)
 * @param axis The height-axis of the cylinder
 */
V3D cylBaseCentre(const std::vector<double> &cylCentre, double height,
                  const std::vector<double> &axis) {
  using Kernel::V3D;
  V3D axisVector = V3D{axis[0], axis[1], axis[2]};
  axisVector.normalize();
  return V3D(cylCentre[0], cylCentre[1], cylCentre[2]) -
         axisVector * height * 0.5;
}

/**
 * Create the xml tag require for a given axis index
 * @param axisIdx Index 0,1,2 for the axis of a cylinder
 * @return A string containing the axis tag for this index
 */
std::string axisXML(unsigned axisIdx) {
  switch (axisIdx) {
  case 0:
    return R"(<axis x="1" y="0" z="0" />)";
  case 1:
    return R"(<axis x="0" y="1" z="0" />)";
  case 2:
    return R"(<axis x="0" y="0" z="1" />)";
  default:
    return "";
  }
}

/**
 * Create the xml tag require for a given axis
 * @param axis 3D vector of double
 * @return A string containing the axis tag representation
 */
std::string axisXML(const std::vector<double> &axis) {
  std::ostringstream str;
  str << "<axis x=\"" << axis[0] << "\" y=\"" << axis[1] << "\" z=\"" << axis[2]
      << "\" /> ";
  return str.str();
}

/**
 * Return a property as type double if possible. Checks for either a
 * double or an int property and casts accordingly
 * @param args A reference to the property manager
 * @param name The name of the property
 * @return The value of the property as a double
 * @throws Exception::NotFoundError if the property does not exist
 */
double getPropertyAsDouble(const Kernel::PropertyManager &args,
                           const std::string &name) {
  try {
    return args.getProperty(name);
  } catch (std::runtime_error &) {
    return static_cast<int>(args.getProperty(name));
  }
}

/**
 * Return a property as type vector<double> if possible. Checks for either a
 * vector<double> or a vector<int> property and casts accordingly
 * @param args A reference to the property manager
 * @param name The name of the property
 * @return The value of the property as a double
 * @throws Exception::NotFoundError if the property does not exist
 */
std::vector<double>
getPropertyAsVectorDouble(const Kernel::PropertyManager &args,
                          const std::string &name) {
  try {
    return args.getProperty(name);
  } catch (std::runtime_error &) {
    std::vector<int> intValues = args.getProperty(name);
    std::vector<double> dblValues(std::begin(intValues), std::end(intValues));
    return dblValues;
  }
}

bool existsAndNotEmptyString(const PropertyManager &pm,
                             const std::string &name) {
  if (pm.existsProperty(name)) {
    const auto value = pm.getPropertyValue(name);
    return !value.empty();
  }
  return false;
}

bool existsAndNegative(const PropertyManager &pm, const std::string &name) {
  if (pm.existsProperty(name)) {
    const auto value = pm.getPropertyValue(name);
    if (boost::lexical_cast<double>(value) < 0.0) {
      return true;
    }
  }
  return false;
}

} // namespace

// Register the algorithm into the AlgorithmFactory
DECLARE_ALGORITHM(SetSample)

/// Algorithms name for identification. @see Algorithm::name
const std::string SetSample::name() const { return "SetSample"; }

/// Algorithm's version for identification. @see Algorithm::version
int SetSample::version() const { return 1; }

/// Algorithm's category for identification. @see Algorithm::category
const std::string SetSample::category() const { return "Sample"; }

/// Algorithm's summary for use in the GUI and help. @see Algorithm::summary
const std::string SetSample::summary() const {
  return "Set properties of the sample and its environment for a workspace";
}

void SetSample::validateGeometry(std::map<std::string, std::string> &errors,
                                 const Kernel::PropertyManager &geomArgs,
                                 const std::string &flavour) {

  // Validate as much of the shape information as possible
  if (existsAndNotEmptyString(geomArgs, GeometryArgs::SHAPE)) {
    auto shape = geomArgs.getPropertyValue(GeometryArgs::SHAPE);
    if (shape == ShapeArgs::CSG) {
      if (!existsAndNotEmptyString(geomArgs, GeometryArgs::VALUE)) {
        errors[flavour] =
            "For " + shape + " shape " + GeometryArgs::VALUE + " is required";
      } else {
        // check if the value is a valid shape XML
        ShapeFactory shapeFactory;
        auto shape = shapeFactory.createShape(
            geomArgs.getPropertyValue(GeometryArgs::VALUE));
        if (!shape || !shape->hasValidShape()) {
          errors[flavour] = "Invalid XML for CSG shape value";
        }
      }
    } else {
      // for the predefined 3 shapes height is mandatory
      if (!existsAndNotEmptyString(geomArgs, ShapeArgs::HEIGHT)) {
        errors[flavour] =
            "For " + shape + " shape " + ShapeArgs::HEIGHT + " is required";
      }
      if (shape == ShapeArgs::FLAT_PLATE ||
          shape == ShapeArgs::FLAT_PLATE_HOLDER) {
        if (!existsAndNotEmptyString(geomArgs, ShapeArgs::WIDTH)) {
          errors[flavour] =
              "For " + shape + " shape " + ShapeArgs::WIDTH + " is required";
        }
        if (!existsAndNotEmptyString(geomArgs, ShapeArgs::THICK)) {
          errors[flavour] =
              "For " + shape + " shape " + ShapeArgs::THICK + " is required";
        }
      }
      if (shape == ShapeArgs::CYLINDER) {
        if (!existsAndNotEmptyString(geomArgs, ShapeArgs::RADIUS)) {
          errors[flavour] =
              "For " + shape + " shape " + ShapeArgs::RADIUS + " is required";
        }
      }
      if (shape == ShapeArgs::HOLLOW_CYLINDER ||
          shape == ShapeArgs::HOLLOW_CYLINDER_HOLDER) {
        if (!existsAndNotEmptyString(geomArgs, ShapeArgs::INNER_RADIUS)) {
          errors[flavour] = "For " + shape + " shape " +
                            ShapeArgs::INNER_RADIUS + " is required";
        }
        if (!existsAndNotEmptyString(geomArgs, ShapeArgs::OUTER_RADIUS)) {
          errors[flavour] = "For " + shape + " shape " +
                            ShapeArgs::OUTER_RADIUS + " is required";
        }
      }
      if (shape == ShapeArgs::FLAT_PLATE_HOLDER) {
        if (!existsAndNotEmptyString(geomArgs, ShapeArgs::WIDTH)) {
          errors[flavour] =
              "For " + shape + " shape " + ShapeArgs::WIDTH + " is required";
        }
        if (!existsAndNotEmptyString(geomArgs, ShapeArgs::FRONT_THICK)) {
          errors[flavour] = "For " + shape + " shape " +
                            ShapeArgs::FRONT_THICK + " is required";
        }
        if (!existsAndNotEmptyString(geomArgs, ShapeArgs::BACK_THICK)) {
          errors[flavour] = "For " + shape + " shape " + ShapeArgs::BACK_THICK +
                            " is required";
        }
      }
      if (shape == ShapeArgs::HOLLOW_CYLINDER_HOLDER) {
        if (!existsAndNotEmptyString(geomArgs, ShapeArgs::INNER_OUTER_RADIUS)) {
          errors[flavour] = "For " + shape + " shape " +
                            ShapeArgs::INNER_OUTER_RADIUS + " is required";
        }
        if (!existsAndNotEmptyString(geomArgs, ShapeArgs::OUTER_INNER_RADIUS)) {
          errors[flavour] = "For " + shape + " shape " +
                            ShapeArgs::OUTER_INNER_RADIUS + " is required";
        }
      }
    }
  } else {
    errors[flavour] = GeometryArgs::SHAPE + " is required";
  }
}

void SetSample::validateMaterial(std::map<std::string, std::string> &errors,
                                 const Kernel::PropertyManager &args,
                                 const std::string &flavour) {
  ReadMaterial::MaterialParameters materialParams;
  setMaterial(materialParams, args);
  auto materialErrors = ReadMaterial::validateInputs(materialParams);
  if (!materialErrors.empty()) {
    std::stringstream ss;
    for (const auto &error : materialErrors) {
      ss << error.first << ":" << error.second << "\n";
    }
    errors[flavour] = ss.str();
  }
}

void SetSample::assertNonNegative(
    std::map<std::string, std::string> &errors,
    const Kernel::PropertyManager &geomArgs, const std::string &flavour,
    const std::vector<const std::string *> &keys) {
  if (existsAndNotEmptyString(geomArgs, GeometryArgs::SHAPE)) {
    for (const auto &arg : keys) {
      if (existsAndNegative(geomArgs, *arg)) {
        errors[flavour] = *arg + " argument < 0.0";
      }
    }
  }
}

/// Validate the inputs against each other @see Algorithm::validateInputs
std::map<std::string, std::string> SetSample::validateInputs() {
  std::map<std::string, std::string> errors;
  // Check workspace type has ExperimentInfo fields
  using API::ExperimentInfo_sptr;
  using API::Workspace_sptr;
  Workspace_sptr inputWS = getProperty(PropertyNames::INPUT_WORKSPACE);
  if (!boost::dynamic_pointer_cast<ExperimentInfo>(inputWS)) {
    errors[PropertyNames::INPUT_WORKSPACE] = "InputWorkspace type invalid. "
                                             "Expected MatrixWorkspace, "
                                             "PeaksWorkspace.";
  }

  const PropertyManager_const_sptr geomArgs =
      getProperty(PropertyNames::GEOMETRY);

  const PropertyManager_const_sptr materialArgs =
      getProperty(PropertyNames::MATERIAL);

  const PropertyManager_const_sptr environArgs =
      getProperty(PropertyNames::ENVIRONMENT);

  const PropertyManager_const_sptr canGeomArgs =
      getProperty(PropertyNames::CONTAINER_GEOMETRY);

  const PropertyManager_const_sptr canMaterialArgs =
      getProperty(PropertyNames::CONTAINER_MATERIAL);

  const std::vector<const std::string *> positiveValues = {
      {&ShapeArgs::HEIGHT, &ShapeArgs::WIDTH, &ShapeArgs::THICK,
       &ShapeArgs::RADIUS, &ShapeArgs::INNER_RADIUS, &ShapeArgs::OUTER_RADIUS}};

  if (environArgs) {
    if (!existsAndNotEmptyString(*environArgs, SEArgs::NAME)) {
      errors[PropertyNames::ENVIRONMENT] =
          "Environment flags require a non-empty 'Name' entry.";
    } else {
      if (!existsAndNotEmptyString(*environArgs, SEArgs::CONTAINER)) {
        errors[PropertyNames::ENVIRONMENT] =
            "Environment flags require a non-empty 'Container' entry.";
      } else {
        // If specifying the environment through XML file, we can not strictly
        // validate the sample settings, since only the overriding properties
        // are specified. Hence we just make sure that whatever is specified is
        // at least positive
        if (geomArgs) {
          assertNonNegative(errors, *geomArgs, PropertyNames::GEOMETRY,
                            positiveValues);
        }
      }
    }
  } else {
    // We cannot strictly require geometry and material to be defined
    // simultaneously; it can be that one is defined at a later time
    if (geomArgs) {
      assertNonNegative(errors, *geomArgs, PropertyNames::GEOMETRY,
                        positiveValues);
      validateGeometry(errors, *geomArgs, PropertyNames::GEOMETRY);
    }
    if (materialArgs) {
      validateMaterial(errors, *materialArgs, PropertyNames::MATERIAL);
    }
  }
  if (canGeomArgs) {
    assertNonNegative(errors, *canGeomArgs, PropertyNames::CONTAINER_GEOMETRY,
                      positiveValues);
    validateGeometry(errors, *canGeomArgs, PropertyNames::CONTAINER_GEOMETRY);
  }

  if (canMaterialArgs) {
    validateMaterial(errors, *canMaterialArgs,
                     PropertyNames::CONTAINER_MATERIAL);
  }
  return errors;
}

/**
 * Initialize the algorithm's properties.
 */
void SetSample::init() {
  using API::Workspace;
  using API::WorkspaceProperty;
  using Kernel::Direction;
  using Kernel::PropertyManagerProperty;

  // Inputs
  declareProperty(std::make_unique<WorkspaceProperty<Workspace>>(
                      PropertyNames::INPUT_WORKSPACE, "", Direction::InOut),
                  "A workspace whose sample properties will be updated");
  declareProperty(std::make_unique<PropertyManagerProperty>(
                      PropertyNames::GEOMETRY, Direction::Input),
                  "A dictionary of geometry parameters for the sample.");
  declareProperty(std::make_unique<PropertyManagerProperty>(
                      PropertyNames::MATERIAL, Direction::Input),
                  "A dictionary of material parameters for the sample. See "
                  "SetSampleMaterial for all accepted parameters");
  declareProperty(
      std::make_unique<PropertyManagerProperty>(PropertyNames::ENVIRONMENT,
                                                Direction::Input),
      "A dictionary of parameters to configure the sample environment");
  declareProperty(std::make_unique<PropertyManagerProperty>(
                      PropertyNames::CONTAINER_GEOMETRY, Direction::Input),
                  "A dictionary of geometry parameters for the container.");
  declareProperty(std::make_unique<PropertyManagerProperty>(
                      PropertyNames::CONTAINER_MATERIAL, Direction::Input),
                  "A dictionary of material parameters for the container.");
}

/**
 * Execute the algorithm.
 */
void SetSample::exec() {
  using API::ExperimentInfo_sptr;
  using Kernel::PropertyManager_sptr;

  Workspace_sptr workspace = getProperty(PropertyNames::INPUT_WORKSPACE);
  PropertyManager_sptr environArgs = getProperty(PropertyNames::ENVIRONMENT);
  PropertyManager_sptr geometryArgs = getProperty(PropertyNames::GEOMETRY);
  PropertyManager_sptr materialArgs = getProperty(PropertyNames::MATERIAL);
  PropertyManager_sptr canGeometryArgs =
      getProperty(PropertyNames::CONTAINER_GEOMETRY);
  PropertyManager_sptr canMaterialArgs =
      getProperty(PropertyNames::CONTAINER_MATERIAL);

  // validateInputs guarantees this will be an ExperimentInfo object
  auto experiment = boost::dynamic_pointer_cast<ExperimentInfo>(workspace);
  // The order here is important. Set the environment first. If this
  // defines a sample geometry then we can process the Geometry flags
  // combined with this
  const SampleEnvironment *sampleEnviron(nullptr);
  if (environArgs) {
    sampleEnviron = setSampleEnvironmentFromFile(*experiment, environArgs);
  } else if (canGeometryArgs) {
    sampleEnviron = setSampleEnvironmentFromXML(*experiment, canGeometryArgs,
                                                canMaterialArgs);
  }

  double sampleVolume = 0.;
  if (geometryArgs || sampleEnviron) {
    setSampleShape(*experiment, geometryArgs, sampleEnviron);
    // get the volume back out to use in setting the material
    sampleVolume = CUBIC_METRE_TO_CM * experiment->sample().getShape().volume();
  }

  // Finally the material arguments
  if (materialArgs) {
    // add the sample volume if it was defined/determined
    if (sampleVolume > 0.) {
      const std::string VOLUME_ARG{"SampleVolume"};
      // only add the volume if it isn't already specfied
      if (!materialArgs->existsProperty(VOLUME_ARG)) {
        materialArgs->declareProperty(
            std::make_unique<PropertyWithValue<double>>(VOLUME_ARG,
                                                        sampleVolume));
      }
    }
    runChildAlgorithm("SetSampleMaterial", workspace, *materialArgs);
  }
}

/**
 * Set the requested sample environment on the workspace from the environment
 * file
 * @param exptInfo A reference to the ExperimentInfo to receive the environment
 * @param args The dictionary of flags for the environment
 * @return A pointer to the new sample environment
 */
const Geometry::SampleEnvironment *SetSample::setSampleEnvironmentFromFile(
    API::ExperimentInfo &exptInfo,
    const Kernel::PropertyManager_const_sptr &args) {
  using Kernel::ConfigService;

  const std::string envName = args->getPropertyValue(SEArgs::NAME);
  std::string canName = "";
  if (args->existsProperty(SEArgs::CONTAINER)) {
    canName = args->getPropertyValue(SEArgs::CONTAINER);
  }
  // The specifications need to be qualified by the facility and instrument.
  // Check instrument for name and then lookup facility if facility
  // is unknown then set to default facility & instrument.
  auto instrument = exptInfo.getInstrument();
  const auto &instOnWS = instrument->getName();
  const auto &config = ConfigService::Instance();
  std::string facilityName, instrumentName;
  try {
    const auto &instInfo = config.getInstrument(instOnWS);
    instrumentName = instInfo.name();
    facilityName = instInfo.facility().name();
  } catch (std::runtime_error &) {
    // use default facility/instrument
    facilityName = config.getFacility().name();
    instrumentName = config.getInstrument().name();
  }

  const auto &instDirs = config.getInstrumentDirectories();
  std::vector<std::string> environDirs(instDirs);
  for (auto &direc : environDirs) {
    direc = Poco::Path(direc).append("sampleenvironments").toString();
  }
  auto finder = std::make_unique<SampleEnvironmentSpecFileFinder>(environDirs);
  SampleEnvironmentFactory factory(std::move(finder));
  auto sampleEnviron =
      factory.create(facilityName, instrumentName, envName, canName);
  exptInfo.mutableSample().setEnvironment(std::move(sampleEnviron));
  return &(exptInfo.sample().getEnvironment());
}

/**
 * Set the requested sample environment from shape XML string
 * @param exptInfo A reference to the ExperimentInfo to receive the environment
 * @param args The dictionary of flags for the environment
 * @return A pointer to the new sample environment
 */
const Geometry::SampleEnvironment *SetSample::setSampleEnvironmentFromXML(
    API::ExperimentInfo &exptInfo,
    const Kernel::PropertyManager_const_sptr &canGeomArgs,
    const Kernel::PropertyManager_const_sptr &canMaterialArgs) {
  const auto refFrame = exptInfo.getInstrument()->getReferenceFrame();
  const auto xml = tryCreateXMLFromArgsOnly(*canGeomArgs, *refFrame);
  if (!xml.empty()) {
    ShapeFactory sFactory;
    // Create the object
    auto shape = sFactory.createShape(xml);
    if (shape->hasValidShape()) {
      if (canMaterialArgs) {
        ReadMaterial::MaterialParameters materialParams;
        setMaterial(materialParams, *canMaterialArgs);
        ReadMaterial reader;
        reader.setMaterialParameters(materialParams);
        auto canMaterial = reader.buildMaterial();
        shape->setMaterial(*canMaterial);
      }
      const SampleEnvironment se("unnamed",
                                 boost::make_shared<Container>(shape));
      exptInfo.mutableSample().setEnvironment(
          std::make_unique<SampleEnvironment>(se));
    }
  }
  return &(exptInfo.sample().getEnvironment());
}

/**
 * @brief SetSample::setMaterial Configures a material from the parameters
 * @param materialParams : output material parameters object
 * @param materialArgs : input material arguments
 */
void SetSample::setMaterial(ReadMaterial::MaterialParameters &materialParams,
                            const Kernel::PropertyManager &materialArgs) {
  if (materialArgs.existsProperty("ChemicalFormula")) {
    materialParams.chemicalSymbol =
        materialArgs.getPropertyValue("ChemicalFormula");
  }
  if (materialArgs.existsProperty("AtomicNumber")) {
    materialParams.atomicNumber = materialArgs.getProperty("AtomicNumber");
  }
  if (materialArgs.existsProperty("MassNumber")) {
    materialParams.massNumber = materialArgs.getProperty("MassNumber");
  }
  if (materialArgs.existsProperty("NumberDensity")) {
    materialParams.sampleNumberDensity =
        materialArgs.getProperty("NumberDensity");
  }
  if (materialArgs.existsProperty("ZParameter")) {
    materialParams.zParameter = materialArgs.getProperty("ZParameter");
  }
  if (materialArgs.existsProperty("UnitCellVolume")) {
    materialParams.unitCellVolume = materialArgs.getProperty("UnitCellVolume");
  }
  if (materialArgs.existsProperty("MassDensity")) {
    materialParams.sampleMassDensity = materialArgs.getProperty("MassDensity");
  }
  if (materialArgs.existsProperty("Mass")) {
    materialParams.sampleMass = materialArgs.getProperty("Mass");
  }
  if (materialArgs.existsProperty("Volume")) {
    materialParams.sampleVolume = materialArgs.getProperty("Volume");
  }
  if (materialArgs.existsProperty("CoherentXSection")) {
    materialParams.coherentXSection =
        materialArgs.getProperty("CoherentXSection");
  }
  if (materialArgs.existsProperty("IncoherentXSection")) {
    materialParams.incoherentXSection =
        materialArgs.getProperty("IncoherentXSection");
  }
  if (materialArgs.existsProperty("AttenuationXSection")) {
    materialParams.attenuationXSection =
        materialArgs.getProperty("AttenuationXSection");
  }
  if (materialArgs.existsProperty("ScatteringXSection")) {
    materialParams.scatteringXSection =
        materialArgs.getProperty("ScatteringXSection");
  }
  if (materialArgs.existsProperty("NumberDensityUnit")) {
    const std::string numberDensityUnit =
        materialArgs.getProperty("NumberDensityUnit");
    if (numberDensityUnit == "Atoms") {
      materialParams.numberDensityUnit =
          MaterialBuilder::NumberDensityUnit::Atoms;
    } else {
      materialParams.numberDensityUnit =
          MaterialBuilder::NumberDensityUnit::FormulaUnits;
    }
  }
}

/**
 * @param experiment A reference to the experiment to be affected
 * @param args The user-supplied dictionary of flags
 * @param sampleEnv A pointer to the sample environment if one exists, otherwise
 * null
 * @return A string containing the XML definition of the shape
 */
void SetSample::setSampleShape(API::ExperimentInfo &experiment,
                               const Kernel::PropertyManager_const_sptr &args,
                               const Geometry::SampleEnvironment *sampleEnv) {
  using Geometry::Container;
  /* The sample geometry can be specified in two ways:
     - a known set of primitive shapes with values or CSG string
     - or a <samplegeometry> field sample environment can, with values possible
       overridden by the Geometry flags
  */

  // Try known shapes or CSG first if supplied
  if (args) {
    const auto refFrame = experiment.getInstrument()->getReferenceFrame();
    const auto xml = tryCreateXMLFromArgsOnly(*args, *refFrame);
    if (!xml.empty()) {
      CreateSampleShape::setSampleShape(experiment, xml);
      return;
    }
  }
  // Any arguments in the args dict are assumed to be values that should
  // override the default set by the sampleEnv samplegeometry if it exists
  if (sampleEnv) {
    const auto &can = sampleEnv->getContainer();
    if (sampleEnv->getContainer().hasCustomizableSampleShape()) {
      Container::ShapeArgs shapeArgs;
      if (args) {
        const auto &props = args->getProperties();
        for (const auto &prop : props) {
          // assume in cm
          const double val = getPropertyAsDouble(*args, prop->name());
          shapeArgs.emplace(boost::algorithm::to_lower_copy(prop->name()),
                            val * 0.01);
        }
      }
      auto shapeObject = can.createSampleShape(shapeArgs);
      // Given that the object is a CSG object, set the object
      // directly on the sample ensuring we preserve the
      // material.
      const auto mat = experiment.sample().getMaterial();
      if (auto csgObj =
              boost::dynamic_pointer_cast<Geometry::CSGObject>(shapeObject)) {
        csgObj->setMaterial(mat);
      }
      experiment.mutableSample().setShape(shapeObject);
    } else if (sampleEnv->getContainer().hasFixedSampleShape()) {
      auto shapeObject = can.getSampleShape();

      // apply Goniometer rotation
      // Rotate only implemented on mesh objects so far
      if (typeid(shapeObject) ==
          typeid(boost::shared_ptr<Geometry::MeshObject>)) {
        const std::vector<double> rotationMatrix =
            experiment.run().getGoniometer().getR();
        boost::dynamic_pointer_cast<Geometry::MeshObject>(shapeObject)
            ->rotate(rotationMatrix);
      }

      const auto mat = experiment.sample().getMaterial();
      shapeObject->setMaterial(mat);

      experiment.mutableSample().setShape(shapeObject);
    } else {
      throw std::runtime_error("The can does not define the sample shape. "
                               "Please either provide a 'Shape' argument "
                               "or update the environment definition with "
                               "this information.");
    }
  } else {
    throw std::runtime_error("No sample environment defined, please provide "
                             "a 'Shape' argument to define the sample "
                             "shape.");
  }
}

/**
 * Create the required XML for a given shape type plus its arguments
 * @param args A dict of flags defining the shape
 * @param refFrame Defines the reference frame for the shape
 * @return A string containing the XML if possible or an empty string
 */
std::string
SetSample::tryCreateXMLFromArgsOnly(const Kernel::PropertyManager &args,
                                    const Geometry::ReferenceFrame &refFrame) {
  std::string result;
  if (!args.existsProperty(GeometryArgs::SHAPE)) {
    return result;
  }

  const auto shape = args.getPropertyValue(GeometryArgs::SHAPE);
  if (shape == ShapeArgs::CSG) {
    result = args.getPropertyValue("Value");
  } else if (shape == ShapeArgs::FLAT_PLATE) {
    result = createFlatPlateXML(args, refFrame);
  } else if (boost::algorithm::ends_with(shape, ShapeArgs::CYLINDER)) {
    result = createCylinderLikeXML(
        args, refFrame,
        boost::algorithm::equals(shape, ShapeArgs::HOLLOW_CYLINDER));
  } else if (boost::algorithm::ends_with(shape, ShapeArgs::FLAT_PLATE_HOLDER)) {
    result = createFlatPlateHolderXML(args, refFrame);
  } else if (boost::algorithm::ends_with(shape,
                                         ShapeArgs::HOLLOW_CYLINDER_HOLDER)) {
    result = createHollowCylinderHolderXML(args, refFrame);
  } else {
    std::stringstream msg;
    msg << "Unknown 'Shape' argument '" << shape
        << "' provided in 'Geometry' property. Allowed values are "
        << ShapeArgs::CSG << ", " << ShapeArgs::FLAT_PLATE << ", "
        << ShapeArgs::CYLINDER << ", " << ShapeArgs::HOLLOW_CYLINDER << ", "
        << ShapeArgs::FLAT_PLATE_HOLDER << ", "
        << ShapeArgs::HOLLOW_CYLINDER_HOLDER;
    throw std::invalid_argument(msg.str());
  }
  if (g_log.is(Logger::Priority::PRIO_DEBUG)) {
    g_log.debug("XML shape definition:\n" + result + '\n');
  }
  return result;
}

/**
 * Create the XML required to define a flat plate from the given args
 * @param args A user-supplied dict of args
 * @param refFrame Defines the reference frame for the shape
 * @param id A generic id for the shape element
 * @return The XML definition string
 */
std::string
SetSample::createFlatPlateXML(const Kernel::PropertyManager &args,
                              const Geometry::ReferenceFrame &refFrame,
                              const std::string &id) const {
  // Helper to take 3 coordinates and turn them to a V3D respecting the
  // current reference frame
  auto makeV3D = [&refFrame](double x, double y, double z) {
    V3D v;
    v[refFrame.pointingHorizontal()] = x;
    v[refFrame.pointingUp()] = y;
    v[refFrame.pointingAlongBeam()] = z;
    return v;
  };
  const double widthInCM = getPropertyAsDouble(args, ShapeArgs::WIDTH);
  const double heightInCM = getPropertyAsDouble(args, ShapeArgs::HEIGHT);
  const double thickInCM = getPropertyAsDouble(args, ShapeArgs::THICK);
  // Convert to half-"width" in metres
  const double szX = (widthInCM * 5e-3);
  const double szY = (heightInCM * 5e-3);
  const double szZ = (thickInCM * 5e-3);
  // Contruct cuboid corners. Define points about origin, rotate and then
  // translate to final center position
  auto lfb = makeV3D(szX, -szY, -szZ);
  auto lft = makeV3D(szX, szY, -szZ);
  auto lbb = makeV3D(szX, -szY, szZ);
  auto rfb = makeV3D(-szX, -szY, -szZ);
  if (args.existsProperty(ShapeArgs::ANGLE)) {
    Goniometer gr;
    const auto upAxis = makeV3D(0, 1, 0);
    gr.pushAxis("up", upAxis.X(), upAxis.Y(), upAxis.Z(),
                args.getProperty(ShapeArgs::ANGLE), Geometry::CCW,
                Geometry::angDegrees);
    auto &rotation = gr.getR();
    lfb.rotate(rotation);
    lft.rotate(rotation);
    lbb.rotate(rotation);
    rfb.rotate(rotation);
  }
  std::vector<double> center = args.getProperty(ShapeArgs::CENTER);
  const V3D centrePos(center[0] * 0.01, center[1] * 0.01, center[2] * 0.01);
  // translate to true center after rotation
  lfb += centrePos;
  lft += centrePos;
  lbb += centrePos;
  rfb += centrePos;

  std::ostringstream xmlShapeStream;
  xmlShapeStream << " <cuboid id=\"" << id << "\"> "
                 << "<left-front-bottom-point x=\"" << lfb.X() << "\" y=\""
                 << lfb.Y() << "\" z=\"" << lfb.Z() << "\"  /> "
                 << "<left-front-top-point  x=\"" << lft.X() << "\" y=\""
                 << lft.Y() << "\" z=\"" << lft.Z() << "\"  /> "
                 << "<left-back-bottom-point  x=\"" << lbb.X() << "\" y=\""
                 << lbb.Y() << "\" z=\"" << lbb.Z() << "\"  /> "
                 << "<right-front-bottom-point  x=\"" << rfb.X() << "\" y =\""
                 << rfb.Y() << "\" z=\"" << rfb.Z() << "\"  /> "
                 << "</cuboid>";
  return xmlShapeStream.str();
}

/**
 * Create the XML required to define a flat plate holder from the given args
 * Flate plate holder is a CSG union of 2 flat plates one on each side of the
 * sample
 * The front and back holders are supposed to have the same width and height and
 * angle as the sample Only the centre needs to be calculated taking into
 * account the thichkness of the sample in between
 * @param args A user-supplied dict of args
 * @param refFrame Defines the reference frame for the shape
 * @return The XML definition string
 */
std::string SetSample::createFlatPlateHolderXML(
    const Kernel::PropertyManager &args,
    const Geometry::ReferenceFrame &refFrame) const {

  std::vector<double> centre =
      getPropertyAsVectorDouble(args, ShapeArgs::CENTER);
  const double sampleThickness = getPropertyAsDouble(args, ShapeArgs::THICK);
  const double frontPlateThickness =
      getPropertyAsDouble(args, ShapeArgs::FRONT_THICK);
  const double backPlateThickness =
      getPropertyAsDouble(args, ShapeArgs::BACK_THICK);
  const double angle = degToRad(getPropertyAsDouble(args, ShapeArgs::ANGLE));
  const auto pointingAlongBeam = refFrame.pointingAlongBeam();
  const auto pointingHorizontal = refFrame.pointingHorizontal();
  const int horizontalSign = (centre[pointingHorizontal] > 0) ? -1 : 1;
  const double horizontalProjection = std::fabs(std::sin(angle));
  const double alongBeamProjection = std::fabs(std::cos(angle));

  auto frontPlate = args;
  auto frontCentre = centre;
  const double frontCentreOffset =
      (frontPlateThickness + sampleThickness) * 0.5;
  frontCentre[pointingAlongBeam] -= frontCentreOffset * alongBeamProjection;
  frontCentre[pointingHorizontal] +=
      frontCentreOffset * horizontalProjection * horizontalSign;
  frontPlate.setProperty(ShapeArgs::CENTER, frontCentre);
  const std::string frontPlateXML =
      createFlatPlateXML(frontPlate, refFrame, "front");

  auto backPlate = args;
  auto backCentre = centre;
  const double backCentreOffset = (backPlateThickness + sampleThickness) * 0.5;
  backCentre[pointingAlongBeam] += backCentreOffset * alongBeamProjection;
  backCentre[pointingHorizontal] -=
      backCentreOffset * horizontalProjection * horizontalSign;
  backPlate.setProperty(ShapeArgs::CENTER, backCentre);
  const std::string backPlateXML =
      createFlatPlateXML(backPlate, refFrame, "back");

  return frontPlateXML + backPlateXML + "<algebra val=\"back:front\"/>";
}

/**
 * Create the XML required to define a hollow cylinder holder from the given
 * args Hollow cylinder holder is a CSG union of 2 hollow cylinders one inside,
 * one outside the sample The centre, the axis and the height are assumed to be
 * the same as for the sample Only the inner and outer radii need to be
 * manipulated
 * @param args A user-supplied dict of args
 * @param refFrame Defines the reference frame for the shape
 * @return The XML definition string
 */
std::string SetSample::createHollowCylinderHolderXML(
    const Kernel::PropertyManager &args,
    const Geometry::ReferenceFrame &refFrame) const {
  auto innerCylinder = args;
  const double innerOuterRadius =
      getPropertyAsDouble(args, ShapeArgs::INNER_OUTER_RADIUS);
  innerCylinder.setProperty(ShapeArgs::OUTER_RADIUS, innerOuterRadius);
  const std::string innerCylinderXML =
      createCylinderLikeXML(innerCylinder, refFrame, true, "inner");
  auto outerCylinder = args;
  const double outerInnerRadius =
      getPropertyAsDouble(args, ShapeArgs::OUTER_INNER_RADIUS);
  outerCylinder.setProperty(ShapeArgs::INNER_RADIUS, outerInnerRadius);
  const std::string outerCylinderXML =
      createCylinderLikeXML(outerCylinder, refFrame, true, "outer");
  return innerCylinderXML + outerCylinderXML + "<algebra val=\"inner:outer\"/>";
}

/**
 * Create the XML required to define a cylinder from the given args
 * @param args A user-supplied dict of args
 * @param refFrame Defines the reference frame for the shape
 * @param hollow True if an annulus is to be created
 * @param id A generic id for the shape element
 * @return The XML definition string
 */
std::string
SetSample::createCylinderLikeXML(const Kernel::PropertyManager &args,
                                 const Geometry::ReferenceFrame &refFrame,
                                 bool hollow, const std::string &id) const {
  const std::string tag = hollow ? "hollow-cylinder" : "cylinder";
  double height = getPropertyAsDouble(args, ShapeArgs::HEIGHT);
  double innerRadius =
      hollow ? getPropertyAsDouble(args, ShapeArgs::INNER_RADIUS) : 0.0;
  double outerRadius = hollow
                           ? getPropertyAsDouble(args, ShapeArgs::OUTER_RADIUS)
                           : getPropertyAsDouble(args, "Radius");
  std::vector<double> centre =
      getPropertyAsVectorDouble(args, ShapeArgs::CENTER);
  // convert to metres
  height *= 0.01;
  innerRadius *= 0.01;
  outerRadius *= 0.01;
  std::transform(centre.begin(), centre.end(), centre.begin(),
                 [](double val) { return val *= 0.01; });
  // XML needs center position of bottom base but user specifies center of
  // cylinder
  V3D baseCentre;
  std::ostringstream XMLString;
  if (args.existsProperty(ShapeArgs::AXIS)) {
    const std::string axis = args.getPropertyValue(ShapeArgs::AXIS);
    if (axis.length() == 1) {
      const auto axisId = static_cast<unsigned>(std::stoi(axis));
      XMLString << axisXML(axisId);
      baseCentre = cylBaseCentre(centre, height, axisId);
    } else {
      const std::vector<double> axis =
          getPropertyAsVectorDouble(args, ShapeArgs::AXIS);
      XMLString << axisXML(axis);
      baseCentre = cylBaseCentre(centre, height, axis);
    }
  } else {
    const auto axisId = static_cast<unsigned>(refFrame.pointingUp());
    XMLString << axisXML(axisId);
    baseCentre = cylBaseCentre(centre, height, axisId);
  }

  std::ostringstream xmlShapeStream;
  xmlShapeStream << "<" << tag << " id=\"" << id << "\"> "
                 << "<centre-of-bottom-base x=\"" << baseCentre.X() << "\" y=\""
                 << baseCentre.Y() << "\" z=\"" << baseCentre.Z() << "\" /> "
                 << XMLString.str() << "<height val=\"" << height << "\" /> ";
  if (hollow) {
    xmlShapeStream << "<inner-radius val=\"" << innerRadius << "\"/>"
                   << "<outer-radius val=\"" << outerRadius << "\"/>";
  } else {
    xmlShapeStream << "<radius val=\"" << outerRadius << "\"/>";
  }
  xmlShapeStream << "</" << tag << ">";
  return xmlShapeStream.str();
}

/**
 * Run the named child algorithm on the given workspace. It assumes an in/out
 * workspace property called InputWorkspace
 * @param name The name of the algorithm to run
 * @param workspace A reference to the workspace
 * @param args A PropertyManager specifying the required arguments
 */
void SetSample::runChildAlgorithm(const std::string &name,
                                  API::Workspace_sptr &workspace,
                                  const Kernel::PropertyManager &args) {
  auto alg = createChildAlgorithm(name);
  alg->setProperty(PropertyNames::INPUT_WORKSPACE, workspace);
  alg->updatePropertyValues(args);
  alg->executeAsChildAlg();
}

} // namespace DataHandling
} // namespace Mantid
