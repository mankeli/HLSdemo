"""Test paneling"""
from hlslib.tile import TILE_SIZE

# pylint: disable=W0621,W0611
from .fixtures import multi_panel_array, one_big_panel_array  # noqa: F401,F811


def test_single_panel(one_big_panel_array):  # noqa: F811
    """Test single panel"""
    assert len(one_big_panel_array.panels) == 1
    expected_tiles = one_big_panel_array.size[0] / TILE_SIZE[0] * one_big_panel_array.size[1] / TILE_SIZE[1]
    assert expected_tiles == 72


def test_multiple_panels(multi_panel_array):  # noqa: F811
    """Test single panel"""
    assert len(multi_panel_array.panels) == 8
    total_tiles = 0
    for panel in multi_panel_array.panels:
        expected_tiles = panel.size[0] / TILE_SIZE[0] * panel.size[1] / TILE_SIZE[1]
        assert expected_tiles == 12
        assert len(panel.tiles) == expected_tiles
        total_tiles += len(panel.tiles)
    expected_tiles = multi_panel_array.size[0] / TILE_SIZE[0] * multi_panel_array.size[1] / TILE_SIZE[1]
    assert expected_tiles == 96
    assert total_tiles == expected_tiles
