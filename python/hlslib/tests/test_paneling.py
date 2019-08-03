"""Test paneling"""
# pylint: disable=W0621,W0611
from .fixtures import multi_panel_array, one_big_panel_array  # noqa: F401,F811


def test_single_panel(one_big_panel_array):  # noqa: F811
    """Test single panel"""
    assert len(one_big_panel_array.panels) == 1


def test_multiple_panels(multi_panel_array):  # noqa: F811
    """Test single panel"""
    assert len(multi_panel_array.panels) == 8
