import unittest
import sys
from sans.gui_logic.presenter.summation_settings_presenter import SummationSettingsPresenter
from sans.gui_logic.models.summation_settings_model import SummationSettings
from sans.gui_logic.models.binning_type import BinningType
from ui.sans_isis.summation_settings_widget import SummationSettingsWidget
from fake_signal import FakeSignal

if sys.version_info.major == 3:
     from unittest import mock
else:
     import mock

class SummationSettingsPresenterTest(unittest.TestCase):
    def setUp(self):
        self.view = self._make_mock_view()
        self.summation_settings = self._make_mock_settings()
        self.presenter = self._make_presenter(self.summation_settings, self.view)

    def _make_mock_view(self):
        mock_view = mock.create_autospec(SummationSettingsWidget, spec_set=True)
        mock_view.binningTypeChanged = FakeSignal()
        mock_view.preserveEventsChanged = FakeSignal()
        mock_view.sum = FakeSignal()
        return mock_view

    def _make_mock_settings(self):
        return mock.create_autospec(SummationSettings)

    def _make_presenter(self, summation_settings, view):
        return SummationSettingsPresenter(summation_settings, view, None)

    def test_sets_binning_type_when_changed(self):
        new_binning_type = BinningType.Custom
        self.view.binningTypeChanged.emit(new_binning_type)
        self.summation_settings.set_histogram_binning_type.assert_called_with(BinningType.Custom)

    def test_retrieves_additional_time_shifts_when_getting_settings_in_event_workspace_mode(self):
        new_binning_type = BinningType.SaveAsEventData
        self.view.binningTypeChanged.emit(new_binning_type)
        self.view.additional_time_shifts.assert_called()

    def test_retrieves_bin_settings_when_getting_settings_in_custom_binning_mode(self):
        new_binning_type = BinningType.Custom
        self.view.binningTypeChanged.emit(new_binning_type)
        self.view.bin_settings.assert_called()

    def test_refreshes_view_when_binning_type_changed(self):
        new_binning_type = BinningType.Custom
        self.view.binningTypeChanged.emit(new_binning_type)
        self.assertEqual(2, self.view.apply_settings.call_count)
        self.view.apply_settings.assert_called_with(self.summation_settings)

    def test_refreshes_view_when_overlay_event_workspaces_changed(self):
        self.view.preserveEventsChanged.emit(True)
        self.assertEqual(2, self.view.apply_settings.call_count)
        self.view.apply_settings.assert_called_with(self.summation_settings)

    def test_enables_overlay_workspace_when_enabled(self):
        self.view.preserveEventsChanged.emit(True)
        self.summation_settings.enable_overlay_event_workspaces.assert_called()

    def test_disabled_overay_workspace_when_disabled(self):
        self.view.preserveEventsChanged.emit(False)
        self.summation_settings.disable_overlay_event_workspaces.assert_called()

if __name__ == '__main__': unittest.main()
