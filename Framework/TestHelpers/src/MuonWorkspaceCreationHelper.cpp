#include "MantidTestHelpers/MuonWorkspaceCreationHelper.h"
#include "MantidTestHelpers/ComponentCreationHelper.h"
#include "MantidTestHelpers/InstrumentCreationHelper.h"
#include "MantidTestHelpers/WorkspaceCreationHelper.h"

#include "MantidMuon/MuonAlgorithmHelper.h"

#include "MantidAPI/FrameworkManager.h"
#include "MantidAPI/GroupingLoader.h"
#include "MantidAPI/MatrixWorkspace.h"
#include "MantidAPI/ScopedWorkspace.h"
#include "MantidAPI/TableRow.h"
#include "MantidAPI/Workspace.h"
#include "MantidAPI/WorkspaceGroup.h"
#include "MantidAlgorithms/CompareWorkspaces.h"
#include "MantidDataHandling/LoadMuonNexus2.h"
#include "MantidDataObjects/TableWorkspace.h"
#include "MantidKernel/PhysicalConstants.h"

using namespace Mantid;
using namespace Mantid::Kernel;
using namespace Mantid::API;
using namespace ScopedFileHelper;

namespace MuonWorkspaceCreationHelper {

// Generate y-values which increment by 1 each time the function is called
yDataCounts2::yDataCounts2() : m_count(-1) {}
double yDataCounts2::operator()(const double, size_t) {
  m_count++;
  return static_cast<double>(m_count);
}

/**
 * Create y-values representing muon data, each spectrum is offset by 4 degrees
 * in phase and has a different normalization. Counts are capped at zero to
 * prevent negative values.
 */
yDataAsymmetry::yDataAsymmetry(const double amp, const double phi)
    : m_amp(amp), m_phi(phi) {}
yDataAsymmetry::yDataAsymmetry() {
  m_amp = 1.5;
  m_phi = 0.1;
}
double yDataAsymmetry::operator()(const double t, size_t spec) {
  double e = exp(-t / tau);
  double factor = (static_cast<double>(spec) + 1.0) * 0.5;
  double phase_offset = 4 * M_PI / 180;
  return std::max(0.0, (10. * factor * (1.0 +
                                        m_amp * cos(m_omega * t + m_phi +
                                                    static_cast<double>(spec) *
                                                        phase_offset)) *
                        e));
}

// Errors are fixed to 0.005
double eData::operator()(const double, size_t) { return 0.005; }

/**
 * Create a matrix workspace appropriate for Group Asymmetry. One detector per
 * spectra, numbers starting from 1. The detector ID and spectrum number are
 * equal.
 * @param nspec :: The number of spectra
 * @param maxt ::  The number of histogram bin edges (between 0.0 and 1.0).
 * Number of bins = maxt - 1 .
 * @param dataGenerator :: A function object which takes a double (t) and
 * integer (spectrum number) and gives back a double (the y-value for the data).
 * @return Pointer to the workspace.
 */
template <typename Function>
MatrixWorkspace_sptr createAsymmetryWorkspace(std::size_t nspec,
                                              std::size_t maxt,
                                              Function dataGenerator) {
  MatrixWorkspace_sptr ws =
      WorkspaceCreationHelper::create2DWorkspaceFromFunction(
          dataGenerator, static_cast<int>(nspec), 0.0, 1.0,
          (1.0 / static_cast<double>(maxt)), true, eData());

  for (auto g = 0u; g < nspec; ++g) {
    auto &spec = ws->getSpectrum(g);
    spec.addDetectorID(g + 1);
    spec.setSpectrumNo(g + 1);
  }
  // Add number of good frames (required for Asymmetry calculation)
  ws->mutableRun().addProperty("goodfrm", 10);
  // Add instrument and run number
  boost::shared_ptr<Geometry::Instrument> inst1 =
      boost::make_shared<Geometry::Instrument>();
  inst1->setName("EMU");
  ws->setInstrument(inst1);
  ws->mutableRun().addProperty("run_number", 12345);

  return ws;
}

Mantid::API::MatrixWorkspace_sptr createAsymmetryWorkspace(size_t nspec,
                                                           size_t maxt) {
  return createAsymmetryWorkspace(nspec, maxt, yDataAsymmetry());
}

/**
 * Create a matrix workspace appropriate for Group Counts. One detector per
 * spectra, numbers starting from 1. The detector ID and spectrum number are
 * equal. Y values increase from 0 in integer steps.
 * @param nspec :: The number of spectra
 * @param maxt ::  The number of histogram bin edges (between 0.0 and 1.0).
 * Number of bins = maxt - 1 .
 * @param seed :: Number added to all y-values.
 * @return Pointer to the workspace.
 */
MatrixWorkspace_sptr createCountsWorkspace(size_t nspec, size_t maxt,
                                           double seed) {

  MatrixWorkspace_sptr ws =
      WorkspaceCreationHelper::create2DWorkspaceFromFunction(
          yDataCounts2(), static_cast<int>(nspec), 0.0, 1.0,
          (1.0 / static_cast<double>(maxt)), true, eData());

  ws->setInstrument(ComponentCreationHelper::createTestInstrumentCylindrical(
      static_cast<int>(nspec)));

  for (int g = 0; g < static_cast<int>(nspec); g++) {
    auto &spec = ws->getSpectrum(g);
    spec.addDetectorID(g + 1);
    spec.setSpectrumNo(g + 1);
    ws->mutableY(g) += seed;
  }

  // Add number of good frames (required for Asymmetry calculation)
  ws->mutableRun().addProperty("goodfrm", 10);
  // Add instrument and run number
  boost::shared_ptr<Geometry::Instrument> inst1 =
      boost::make_shared<Geometry::Instrument>();
  inst1->setName("EMU");
  ws->setInstrument(inst1);
  ws->mutableRun().addProperty("run_number", 12345);

  return ws;
}

/**
 * Create a WorkspaceGroup and add to the ADS, populate with MatrixWorkspaces
 * simulating periods as used in muon analysis. Workspace for period i has a
 * name ending _i.
 * @param nPeriods :: The number of periods (independent workspaces).
 * @param nspec :: The number of spectra in each workspace.
 * @param maxt ::  The number of histogram bin edges (between 0.0 and 1.0).
 * Number of bins = maxt - 1 .
 * @param wsGroupName :: Name of the workspace group containing the period
 * workspaces.
 * @return Pointer to the workspace group.
 */
WorkspaceGroup_sptr
createMultiPeriodWorkspaceGroup(const int &nPeriods, size_t nspec, size_t maxt,
                                const std::string &wsGroupName) {

  WorkspaceGroup_sptr wsGroup = boost::make_shared<WorkspaceGroup>();
  AnalysisDataService::Instance().addOrReplace(wsGroupName, wsGroup);

  std::string wsNameStem = "MuonDataPeriod_";
  std::string wsName;

  boost::shared_ptr<Geometry::Instrument> inst1 =
      boost::make_shared<Geometry::Instrument>();
  inst1->setName("EMU");

  for (int period = 1; period < nPeriods + 1; period++) {
    // Period 1 yvalues : 1,2,3,4,5,6,7,8,9,10
    // Period 2 yvalues : 2,3,4,5,6,7,8,9,10,11 etc..
    MatrixWorkspace_sptr ws = createCountsWorkspace(nspec, maxt, period);

    wsGroup->addWorkspace(ws);
    wsName = wsNameStem + std::to_string(period);
    AnalysisDataService::Instance().addOrReplace(wsName, ws);
  }

  return wsGroup;
}

/**
 * Create a simple dead time TableWorkspace with two columns (spectrum number
 * and dead time).
 * @param nspec :: The number of spectra (rows in table).
 * @param deadTimes ::  The dead times for each spectra.
 * @return TableWorkspace with dead times appropriate for pairing algorithm.
 */
ITableWorkspace_sptr createDeadTimeTable(const size_t &nspec,
                                         std::vector<double> &deadTimes) {

  auto deadTimeTable = boost::dynamic_pointer_cast<ITableWorkspace>(
      WorkspaceFactory::Instance().createTable("TableWorkspace"));

  deadTimeTable->addColumn("int", "Spectrum Number");
  deadTimeTable->addColumn("double", "Dead Time");

  if (deadTimes.size() != nspec) {
    return deadTimeTable;
  }

  for (size_t spec = 0; spec < deadTimes.size(); spec++) {
    TableRow newRow = deadTimeTable->appendRow();
    newRow << static_cast<int>(spec) + 1;
    newRow << deadTimes[spec];
  }

  return deadTimeTable;
}

/**
 * Simplest possible grouping file, with only a single group.
 * @param groupName :: Name of the group.
 * @param group :: Detector grouping string (e.g. "1-4,5,6-10").
 * @return ScopedFile object.
 */
ScopedFileHelper::ScopedFile
createGroupingXMLSingleGroup(const std::string &groupName,
                             const std::string &group) {
  std::string fileContents("");
  fileContents += "<detector-grouping description=\"test XML file\"> \n";
  fileContents += "\t<group name=\"" + groupName + "\"> \n";
  fileContents += "\t\t<ids val=\"" + group + "\"/>\n";
  fileContents += "\t</group>\n";
  fileContents += "\t<default name=\"" + groupName + "\"/>\n";
  fileContents += "</detector-grouping>";

  ScopedFileHelper::ScopedFile file(fileContents, "testXML_1.xml");
  return file;
}

/**
 * Grouping file with two groups (group1 and group2), combined into one pair.
 * @param pairName :: Name of the pair.
 * @param groupName :: Override the name of the second group of the pair, used
 * to test a possible failure case.
 * @return ScopedFile object.
 */
ScopedFileHelper::ScopedFile
createGroupingXMLSinglePair(const std::string &pairName,
                            const std::string &groupName) {

  std::string fileContents("");

  fileContents += "<detector-grouping description=\"test XML file\"> \n";
  fileContents += "\t<group name=\"group1\"> \n";
  fileContents += "\t\t<ids val=\"1\"/>\n";
  fileContents += "\t</group>\n";

  fileContents += "<detector-grouping description=\"test XML file\"> \n";
  fileContents += "\t<group name=\"group2\"> \n";
  fileContents += "\t\t<ids val=\"2\"/>\n";
  fileContents += "\t</group>\n";

  fileContents += "\t<pair name=\"" + pairName + "\"> \n";
  fileContents += "\t\t<forward-group val=\"group1\"/>\n";
  fileContents += "\t\t<backward-group val=\"" + groupName + "\"/>\n";
  fileContents += "\t\t<alpha val=\"1\"/>\n";
  fileContents += "\t</pair>\n";

  fileContents += "\t<default name=\"" + groupName + "\"/>\n";
  fileContents += "</detector-grouping>";

  ScopedFileHelper::ScopedFile file(fileContents, "testXML_1.xml");

  return file;
}

/**
 * Create an XML file with grouping/pairing information. With nGroups = 3 and
 * nDetectorPerGroup = 5 the grouping would be {"1-5","6-10","11-15"}.
 *
 * @param nGroups :: The number of groups to produce
 * @param nDetectorsPerGroup ::  The number of detector IDs per group
 * @return ScopedFile.
 */
ScopedFileHelper::ScopedFile
createXMLwithPairsAndGroups(const int &nGroups, const int &nDetectorsPerGroup) {

  API::Grouping grouping;
  std::string groupIDs;
  // groups
  for (auto group = 1; group <= nGroups; group++) {
    std::string groupName = "group" + std::to_string(group);
    if (nGroups == 1) {
      groupIDs = "1";
    } else {
      groupIDs = std::to_string((group - 1) * nDetectorsPerGroup + 1) + "-" +
                 std::to_string(group * nDetectorsPerGroup);
    }
    grouping.groupNames.emplace_back(groupName);
    grouping.groups.emplace_back(groupIDs);
  }
  // pairs
  for (auto pair = 1; pair < nGroups; pair++) {
    std::string pairName = "pair" + std::to_string(pair);
    std::pair<size_t, size_t> pairIndices;
    pairIndices.first = 0;
    pairIndices.second = pair;
    grouping.pairNames.emplace_back(pairName);
    grouping.pairAlphas.emplace_back(1.0);
    grouping.pairs.emplace_back(pairIndices);
  }

  auto fileContents = MuonAlgorithmHelper::groupingToXML(grouping);
  ScopedFileHelper::ScopedFile file(fileContents, "testXML_1.xml");
  return file;
}

} // namespace MuonWorkspaceCreationHelper
