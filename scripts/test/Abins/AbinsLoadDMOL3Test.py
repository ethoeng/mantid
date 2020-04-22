# Mantid Repository : https://github.com/mantidproject/mantid
#
# Copyright &copy; 2018 ISIS Rutherford Appleton Laboratory UKRI,
#   NScD Oak Ridge National Laboratory, European Spallation Source,
#   Institut Laue - Langevin & CSNS, Institute of High Energy Physics, CAS
# SPDX - License - Identifier: GPL - 3.0 +
import unittest
from mantid.simpleapi import logger
from abins import GeneralLoadAbInitioTester, LoadDMOL3, test_helpers


class AbinsLoadDMOL3Test(unittest.TestCase, GeneralLoadAbInitioTester):

    def tearDown(self):
        test_helpers.remove_output_files(list_of_names=["LoadDMOL3"])

        #  *************************** USE CASES ********************************************
    # ===================================================================================
    # | Use cases: Gamma point calculation for DMOL3                                    |
    # ===================================================================================
    _gamma_dmol3 = "LTA_40_O2_LoadDMOL3"
    _gamma_no_h_dmol3 = "Na2SiF6_LoadDMOL3"

    def test_gamma_dmol3(self):
        self.check(name=self._gamma_dmol3, loader=LoadDMOL3)
        self.check(name=self._gamma_no_h_dmol3, loader=LoadDMOL3)

if __name__ == '__main__':
    unittest.main()
