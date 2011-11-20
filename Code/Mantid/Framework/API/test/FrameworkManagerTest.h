#ifndef FRAMEWORKMANAGERTEST_H_
#define FRAMEWORKMANAGERTEST_H_

#include <cxxtest/TestSuite.h>

#include "MantidAPI/FrameworkManager.h"
#include "MantidAPI/Algorithm.h"
#include "MantidAPI/AlgorithmFactory.h"
#include <stdexcept>
#include <iostream>

using namespace Mantid::Kernel;
using namespace Mantid::API;

class ToyAlgorithm2 : public Algorithm
{
public:
  ToyAlgorithm2() {}
  virtual ~ToyAlgorithm2() {}
  virtual const std::string name() const { return "ToyAlgorithm2";};///< Algorithm's name for identification
  virtual int version() const { return 1;};///< Algorithm's version for identification
  void init()
  { declareProperty("Prop","");
    declareProperty("P2","");
    declareProperty("Filename","");    
  }
  void exec() { }
  void final() { }
};

DECLARE_ALGORITHM(ToyAlgorithm2)

using namespace Mantid;

class FrameworkManagerTest : public CxxTest::TestSuite
{
public:

  void testConstructor()
  {
    // Not really much to test
    TS_ASSERT_THROWS_NOTHING( FrameworkManager::Instance() );

#ifdef MPI_BUILD
    // If this is 'MPI Mantid' then test that the mpi environment has been initialized
    TS_ASSERT( boost::mpi::environment::initialized() );
#endif
  }

  void testcreateAlgorithm()
  {
    TS_ASSERT_THROWS_NOTHING( FrameworkManager::Instance().createAlgorithm("ToyAlgorithm2") )
    TS_ASSERT_THROWS( FrameworkManager::Instance().createAlgorithm("ToyAlgorithm2","",3), std::runtime_error )
    TS_ASSERT_THROWS( FrameworkManager::Instance().createAlgorithm("aaaaaa"), std::runtime_error )
  }

  void testcreateAlgorithmWithProps()
  {
    IAlgorithm *alg = FrameworkManager::Instance().createAlgorithm("ToyAlgorithm2","Prop=Val;P2=V2");
    std::string prop;
    TS_ASSERT_THROWS_NOTHING( prop = alg->getPropertyValue("Prop") )
    TS_ASSERT( ! prop.compare("Val") )
    TS_ASSERT_THROWS_NOTHING( prop = alg->getPropertyValue("P2") )
    TS_ASSERT( ! prop.compare("V2") )

    TS_ASSERT_THROWS_NOTHING( FrameworkManager::Instance().createAlgorithm("ToyAlgorithm2","") )
//    TS_ASSERT_THROWS_NOTHING( manager->createAlgorithm("ToyAlgorithm2","noValProp") )
    TS_ASSERT_THROWS( FrameworkManager::Instance().createAlgorithm("ToyAlgorithm2","P1=P2=P3"), std::invalid_argument)
  }

  void testExec()
  {
    IAlgorithm *alg = FrameworkManager::Instance().exec("ToyAlgorithm2","Prop=Val;P2=V2");
    TS_ASSERT( alg->isExecuted() )
  }

  void testGetWorkspace()
  {
    TS_ASSERT_THROWS( FrameworkManager::Instance().getWorkspace("wrongname"), std::runtime_error )
  }
//
//  /** Ticket #4174: this algorithm segfaults at the point where you've
//   * created 50  (the algorithms.retained property) in a row.
//   */
//  void test_CreateAlgorithm_manytimes()
//  {
//    for (size_t i=0; i<100; i++)
//    {
//      std::cout << "Creating algorithm #" << i << std::endl;
//      IAlgorithm * alg = FrameworkManager::Instance().createAlgorithm("CreateWorkspace");
//      // The line below segfaults at i = 50 (the algorithms.retained property)
//      alg->setPropertyValue("OutputWorkspace", "dummy");
//      alg->setPropertyValue("DataX", "1");
//      alg->setPropertyValue("DataY", "2");
//      alg->execute();
//      delete alg;
//      // You can add this sleep line and it still segfaults.
//      //sleep(1);
//    }
//  }

};

#endif /*FRAMEWORKMANAGERTEST_H_*/
