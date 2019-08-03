"""Test tiling"""
import numpy as np

from hlslib.tile import TILE_SIZE

# pylint: disable=W0621,W0611
from .fixtures import (BLACK, BLUE, GREEN, fill_squares,  # noqa: F401,F811
                       get_tile_by_array_position, multi_panel_array,
                       one_big_panel_array)


def test_single_panel_array(one_big_panel_array):  # noqa: F811
    """Test single panel tiling"""
    fill_squares(one_big_panel_array, 30)
    # Some sanity-checks on the main frame matrix
    test_pixel = one_big_panel_array.pixeldata[-1, -1]
    assert np.array_equal(test_pixel, BLUE)
    test_pixel = one_big_panel_array.pixeldata[-30, -30]
    assert np.array_equal(test_pixel, BLUE)
    test_pixel = one_big_panel_array.pixeldata[-31, -31]
    assert np.array_equal(test_pixel, GREEN)
    test_pixel = one_big_panel_array.pixeldata[-31, -1]
    assert np.array_equal(test_pixel, BLACK)
    test_pixel = one_big_panel_array.pixeldata[-1, -31]
    assert np.array_equal(test_pixel, BLACK)


def test_single_panel_last_tile(one_big_panel_array):  # noqa: F811
    """"check that last tile is full of expected color"""
    # Final tile, should be all blue
    fill_squares(one_big_panel_array, 30)
    last_tile = one_big_panel_array.panels[0].tiles[-1]
    assert last_tile.panel_position[0] == 192 - 16
    assert last_tile.panel_position[1] == 96 - 16
    shape = last_tile.pixeldata.shape
    assert np.array_equal(last_tile.pixeldata, np.full(shape, BLUE))


def test_single_panel_next_to_last_tiles(one_big_panel_array):  # noqa: F811
    """Check tiles around the last tile, should be mostly blue but not fully"""
    # Second from the right, should be mostly blue but not all blue
    fill_squares(one_big_panel_array, 30)
    penultimate_tile = get_tile_by_array_position(one_big_panel_array, (192 - 32, 96 - 16))
    assert penultimate_tile
    test_pixel = penultimate_tile.pixeldata[-1, -1]
    assert np.array_equal(test_pixel, BLUE)
    for xpos in range(2):
        for ypos in range(TILE_SIZE[1]):
            test_pixel = penultimate_tile.pixeldata[ypos, xpos]
            assert np.array_equal(test_pixel, BLACK)
    for xpos in range(2, TILE_SIZE[0]):
        for ypos in range(TILE_SIZE[1]):
            test_pixel = penultimate_tile.pixeldata[ypos, xpos]
            assert np.array_equal(test_pixel, BLUE)

    # third from the right bottom, should be all black
    test_tile = get_tile_by_array_position(one_big_panel_array, (192 - 48, 96 - 16))
    assert test_tile
    shape = test_tile.pixeldata.shape
    assert np.array_equal(test_tile.pixeldata, np.zeros(shape=shape))

    # second up from bottom right, mostly blue but not all blue
    penultimate_tile = get_tile_by_array_position(one_big_panel_array, (192 - 16, 96 - 32))
    assert penultimate_tile
    test_pixel = penultimate_tile.pixeldata[-1, -1]
    assert np.array_equal(test_pixel, BLUE)
    for ypos in range(2):
        for xpos in range(TILE_SIZE[0]):
            test_pixel = penultimate_tile.pixeldata[ypos, xpos]
            assert np.array_equal(test_pixel, BLACK)
    for ypos in range(2, TILE_SIZE[1]):
        for xpos in range(TILE_SIZE[0]):
            test_pixel = penultimate_tile.pixeldata[ypos, xpos]
            assert np.array_equal(test_pixel, BLUE)
