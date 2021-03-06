// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2010 ISIS Rutherford Appleton Laboratory UKRI,
//   NScD Oak Ridge National Laboratory, European Spallation Source,
//   Institut Laue - Langevin & CSNS, Institute of High Energy Physics, CAS
// SPDX - License - Identifier: GPL - 3.0 +
#pragma once

/**
  Create markers that will highlight the position of a Peak
  from a PeaksWorkspace.

  Implementation of a vtkDataSetFactory.

  @author Janik Zikovsky
  @date 06/24/2011
 */

#include "MantidAPI/IMDWorkspace.h"
#include "MantidAPI/IPeaksWorkspace_fwd.h"
#include "MantidDataObjects/PeakShapeEllipsoid.h"
#include "MantidGeometry/Crystal/IPeak.h"
#include "MantidVatesAPI/vtkDataSetFactory.h"

class vtkPolyData;

namespace Mantid {
namespace VATES {
// Forward dec.
class ProgressAction;

class DLLExport vtkPeakMarkerFactory {
public:
  /// Enum describing which dimension to show single-crystal peaks
  enum ePeakDimensions {
    Peak_in_Q_lab,    ///< Q in the lab frame
    Peak_in_Q_sample, ///< Q in the sample frame (goniometer rotation taken out)
    Peak_in_HKL       ///< HKL miller indices
  };

  /// Constructor
  vtkPeakMarkerFactory(const std::string &scalarname,
                       ePeakDimensions dimensions = Peak_in_Q_lab);

  /// Initialize the object with a workspace.
  void initialize(Mantid::API::Workspace_sptr workspace);

  /// Factory method
  vtkSmartPointer<vtkPolyData> create(ProgressAction &progressUpdating) const;

  std::string getFactoryTypeName() const { return "vtkPeakMarkerFactory"; }

  /// Getter for the peak workspace integration radius
  double getIntegrationRadius() const;

  /// Was the peaks workspace integrated?
  bool isPeaksWorkspaceIntegrated() const;

protected:
  void validate() const;

private:
  void validateWsNotNull() const;

  void validateDimensionsPresent() const;

  /// Get glyph position
  Kernel::V3D getPosition(const Mantid::Geometry::IPeak &peak) const;

  /// Get ellipsoid axes
  std::vector<Mantid::Kernel::V3D>
  getAxes(const Mantid::DataObjects::PeakShapeEllipsoid &ellipticalShape,
          const Mantid::Geometry::IPeak &peak) const;

  /// Get the tranform tensor for vtkTensorGlyph
  std::array<float, 9> getTransformTensor(
      const Mantid::DataObjects::PeakShapeEllipsoid &ellipticalShape,
      const Mantid::Geometry::IPeak &peak) const;

  /// Peaks workspace containg peaks to mark
  Mantid::API::IPeaksWorkspace_sptr m_workspace;

  /// Name of the scalar to provide on mesh.
  std::string m_scalarName;

  /// Which peak dimension to use
  ePeakDimensions m_dimensionToShow;

  /// peak radius value.
  double m_peakRadius;
};
} // namespace VATES
} // namespace Mantid
