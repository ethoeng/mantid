from mantidqtpython import MantidQt
from mantid import ConfigService
from run_selector_presenter import RunSelectorPresenter
from summation_settings_presenter import SummationSettingsPresenter

class AddRunsPagePresenter(object):
    def __init__(self, run_selection, run_finder, summation_settings, view, parent_view):
        self._run_selection = run_selection
        self._run_finder = run_finder
        self._summation_settings = summation_settings
        self.view = view
        self.parent = parent_view
        self.run_selector_presenter = RunSelectorPresenter(self._run_selection, self._run_finder, self.view.run_selector, self)
        self.summation_settings_presenter = SummationSettingsPresenter(self._summation_settings, self.view.summation_settings, self)
        self.connect_to_view(view)

    def connect_to_view(self, view):
        # view.sum.connect(self.handle_sum)
        pass

    def handle_sum(self):
        print "Sum All Items"
