"""Abstract a panel"""
import numpy as np

from .tile import TILE_SIZE, Tile

PANEL_DEFAULT_SIZE = (128, 96)


class Panel:
    """Abstract a panel"""
    array_position = (0, 0)  # top-left pixel x,y address for this panel
    size = PANEL_DEFAULT_SIZE
    addr = None
    pixeldata = None
    p_array = None
    tiles = None
    connection = None

    def __init__(self, ip, port=9998, pixeldata=None, connection=None,
                 p_array=None, array_position=(0, 0), size=PANEL_DEFAULT_SIZE):
        self.addr = (ip, port)
        self.size = size
        self.p_array = p_array
        self.array_position = array_position

        if connection is not None:
            self.connection = connection
        elif p_array:
            self.connection = p_array.connection

        if pixeldata is not None:
            self.pixeldata = pixeldata
        elif p_array:
            # Remember: numpy uses y,x array indexing
            self.pixeldata = p_array.pixeldata[
                array_position[1]:array_position[1] + self.size[1],
                array_position[0]:array_position[0] + self.size[0]
            ]
        else:
            self.pixeldata = np.zeros(
                shape=(self.size[1], self.size[0]),
                dtype=np.uint16
            )
        self.create_tiles()

    def send_pixels(self, frameno=0):
        """Send this panels tiles"""
        for tile in self.tiles:
            self.connection.send_tile(tile, frameno)

    def create_tiles(self):
        """create the tiles, 2d array x,y addressing"""
        self.tiles = []
        for tilex in range(0, self.size[0], TILE_SIZE[0]):
            for tiley in range(0, self.size[1], TILE_SIZE[1]):
                tile = Tile(self, (tilex, tiley))
                self.tiles.append(tile)
