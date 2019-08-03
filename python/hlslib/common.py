"""Common helpers"""
import numpy as np


def get_pixbuf(size):
    """Returns a properly initialized numpy array for our pixel data"""
    return np.zeros(
        shape=(size[1], size[0], 3),
        dtype=np.uint16
    )


def uint8_to_uint16(input_array):
    """Scale 8-bit colors to 16-bit ones"""
    raise NotImplementedError()
