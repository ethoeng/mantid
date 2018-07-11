from __future__ import absolute_import, print_function

from PyQt4 import QtGui

import sys

from Muon.GUI.ElementalAnalysis.PeriodicTable.periodic_table_widget import PeriodicTable
from Muon.GUI.Common import message_box


class ElementalAnalysisGui(QtGui.QMainWindow):
    def __init__(self, parent=None):
        super(ElementalAnalysisGui, self).__init__(parent)

        self.ptable = PeriodicTable()
        self.ptable.register_table_changed(self.table_changed)
        self.ptable.register_table_clicked(self.table_clicked)
        self.setCentralWidget(self.ptable.widget)
        self.setWindowTitle("Elemental Analysis")

    def table_clicked(self, item):
        print("Element Clicked: {}".format(item.symbol))

    def table_changed(self, items):
        print("Table Changed: {}".format([i.symbol for i in items]))


def qapp():
    if QtGui.QApplication.instance():
        _app = QtGui.QApplication.instance()
    else:
        _app = QtGui.QApplication(sys.argv)
    return _app


app = qapp()
try:
    window = ElementalAnalysisGui()
    window.show()
    app.exec_()
except RuntimeError as error:
    message_box.warning(str(error))
