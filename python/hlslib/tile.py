"""Abstract tiles"""

TILE_SIZE = (16, 16)


class Tile:  # pylint: disable=R0903
    """Abstract tile"""
    pixeldata = None
    panel_position = (0, 0)
    array_position = (0, 0)
    panel = None

    def __repr__(self):
        return '<Tile(panel_position={ppos}, array_position={apos})>'.format(
            ppos=self.panel_position, apos=self.array_position
        )

    def __init__(self, panel, panel_position):
        self.panel = panel
        self.panel_position = panel_position
        self.array_position = (panel.array_position[0] + panel_position[0],
                               panel.array_position[1] + panel_position[1])
        # Remember: numpy uses y,x array indexing
        self.pixeldata = self.panel.pixeldata[
            panel_position[1]:panel_position[1] + 16,
            panel_position[0]:panel_position[0] + 16
        ]
