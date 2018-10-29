# Mantid Repository : https://github.com/mantidproject/mantid
#
# Copyright &copy; 2018 ISIS Rutherford Appleton Laboratory UKRI,
#     NScD Oak Ridge National Laboratory, European Spallation Source
#     & Institut Laue - Langevin
# SPDX - License - Identifier: GPL - 3.0 +
#  This file is part of the mantid workbench.
#
#
import os

from qtpy.QtWidgets import QApplication

from mantid.simpleapi import Load
from mantidqt.widgets.matrixworkspacedisplay.presenter import MatrixWorkspaceDisplay

p = R"C:\Users\qbr77747\dev\m\source\build\ExternalData\Testing\Data\UnitTest"
#
# all_nxs_files = []
# for root, folders, files in os.walk(p):
#     nxs_files = list(filter(lambda x: x[-4:] == ".nxs" in x, files))
#     all_nxs_files.extend(nxs_files)
#
# print(all_nxs_files)

# for f in all_nxs_files:
#     full_path = os.path.join(p, f)
app = QApplication([])
ws = Load(os.path.join(p, r"ARCS_2963.nxs"))
window = MatrixWorkspaceDisplay(ws)
app.exec_()
