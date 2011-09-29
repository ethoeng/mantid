#ifndef SQW_LOADING_PRESENTER_TEST_H_
#define SQW_LOADING_PRESENTER_TEST_H_ 

#include <cxxtest/TestSuite.h>
#include <vtkUnstructuredGrid.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "MockObjects.h"

#include "MantidAPI/FileFinder.h"
#include "MantidVatesAPI/SQWLoadingPresenter.h"
#include "MantidVatesAPI/FilteringUpdateProgressAction.h"

using namespace Mantid::VATES;

//=====================================================================================
// Functional tests
//=====================================================================================
class SQWLoadingPresenterTest : public CxxTest::TestSuite
{

private:

  // Helper method to return the full path to a real sqw file.
  static std::string getSuitableFileNamePath()
  {
    return Mantid::API::FileFinder::Instance().getFullPath("test_horace_reader.sqw");
  }
  
  // Helper method to return the full path to a file that is invalid.
  static std::string getUnhandledFileNamePath()
  {
    return Mantid::API::FileFinder::Instance().getFullPath("emu00006473.nxs");
  }

  // Helper method. Create the expected backend filename + path using the same rules used internally in SQWLoadingPresenter.
  static std::string getFileBackend(std::string fileName)
  {
    size_t pos = fileName.find(".");
    return fileName.substr(0, pos) + ".nxs";
  }

public:

void setUp()
{
    std::remove(getFileBackend(getSuitableFileNamePath()).c_str()); // Clean out any pre-existing backend files.
}

void testConstructWithEmptyFileThrows()
{
  TSM_ASSERT_THROWS("Should throw if an empty file string is given.", SQWLoadingPresenter(new MockMDLoadingView, ""), std::invalid_argument);
}

void testConstructWithNullViewThrows()
{
  MockMDLoadingView*  pView = NULL;
  TSM_ASSERT_THROWS("Should throw if an empty file string is given.", SQWLoadingPresenter(pView, "some_file"), std::invalid_argument);
}

void testConstruct()
{
  TSM_ASSERT_THROWS_NOTHING("Object should be created without exception.", SQWLoadingPresenter(new MockMDLoadingView, getSuitableFileNamePath()));
}

void testCanReadFile()
{
  MockMDLoadingView view;

  SQWLoadingPresenter presenter(new MockMDLoadingView, getSuitableFileNamePath());
  TSM_ASSERT("Should be readable, valid SQW file.", presenter.canReadFile());
}

void testCanReadFileWithDifferentCaseExtension()
{
  SQWLoadingPresenter presenter(new MockMDLoadingView, "other.Sqw");
  TSM_ASSERT("Should be readable, only different in case.", presenter.canReadFile());
}

void testCannotReadFileWithWrongExtension()
{
  SQWLoadingPresenter presenter(new MockMDLoadingView, getUnhandledFileNamePath());
  TSM_ASSERT("Should NOT be readable, completely wrong file type.", !presenter.canReadFile());
}

void testExecutionInMemory()
{
  using namespace testing;
   //Setup view
  MockMDLoadingView* view = new MockMDLoadingView;
  EXPECT_CALL(*view, getRecursionDepth()).Times(AtLeast(1)); 
  EXPECT_CALL(*view, getLoadInMemory()).Times(AtLeast(1)).WillRepeatedly(Return(true)); // View setup to request loading in memory.
  EXPECT_CALL(*view, getTime()).Times(AtLeast(1));
  EXPECT_CALL(*view, updateAlgorithmProgress(_)).Times(AnyNumber());

  //Setup rendering factory
  MockvtkDataSetFactory factory;
  EXPECT_CALL(factory, initialize(_)).Times(1);
  EXPECT_CALL(factory, create()).WillOnce(testing::Return(vtkUnstructuredGrid::New()));
  EXPECT_CALL(factory, setRecursionDepth(_)).Times(1);

  //Setup progress updates object
  FilterUpdateProgressAction<MockMDLoadingView> progressAction(view);

  //Create the presenter and runit!
  SQWLoadingPresenter presenter(view, getSuitableFileNamePath());
  presenter.executeLoadMetadata();
  vtkDataSet* product = presenter.execute(&factory, progressAction);
  
  std::string fileNameIfGenerated = getFileBackend(getSuitableFileNamePath());
  ifstream fileExists(fileNameIfGenerated);
  TSM_ASSERT("File Backend SHOULD NOT be generated.",  !fileExists.good());

  TSM_ASSERT("Should have generated a vtkDataSet", NULL != product);
  TSM_ASSERT_EQUALS("Wrong type of output generated", "vtkUnstructuredGrid", std::string(product->GetClassName()));
  TSM_ASSERT("No field data!", NULL != product->GetFieldData());
  TSM_ASSERT_EQUALS("One array expected on field data!", 1, product->GetFieldData()->GetNumberOfArrays());
  TS_ASSERT_THROWS_NOTHING(presenter.hasTDimensionAvailable());
  TS_ASSERT_THROWS_NOTHING(presenter.getGeometryXML());

  TS_ASSERT(Mock::VerifyAndClearExpectations(view));
  TS_ASSERT(Mock::VerifyAndClearExpectations(&factory));

  product->Delete();
}

void testCallHasTDimThrows()
{
  SQWLoadingPresenter presenter(new MockMDLoadingView, getSuitableFileNamePath());
  TSM_ASSERT_THROWS("Should throw. Execute not yet run.", presenter.hasTDimensionAvailable(), std::runtime_error);
}

void testCallGetTDimensionValuesThrows()
{
  SQWLoadingPresenter presenter(new MockMDLoadingView, getSuitableFileNamePath());
  TSM_ASSERT_THROWS("Should throw. Execute not yet run.", presenter.getTimeStepValues(), std::runtime_error);
}

void testCallGetGeometryThrows()
{
  SQWLoadingPresenter presenter(new MockMDLoadingView, getSuitableFileNamePath());
  TSM_ASSERT_THROWS("Should throw. Execute not yet run.", presenter.getGeometryXML(), std::runtime_error);
}

void testExecuteLoadMetadata()
{
  SQWLoadingPresenter presenter(new MockMDLoadingView, getSuitableFileNamePath());
  presenter.executeLoadMetadata();
  TSM_ASSERT_THROWS_NOTHING("Should throw. Execute not yet run.", presenter.getTimeStepValues());
  TSM_ASSERT_THROWS_NOTHING("Should throw. Execute not yet run.", presenter.hasTDimensionAvailable());
  TSM_ASSERT_THROWS_NOTHING("Should throw. Execute not yet run.", presenter.getGeometryXML());
}

};

#endif