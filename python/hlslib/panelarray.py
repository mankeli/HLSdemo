"""Abstract array of panels"""
import enum

from .common import get_pixbuf
from .connection import Connection
from .panel import Panel


class ConfigParts(enum.IntEnum):
    """Usable names for config tuple parts"""
    startx = 0
    starty = 1
    endx = 2
    endy = 3
    ip = 4
    port = 5


class PanelArray:
    """Abstract array of panels"""
    size = None
    panels = None
    connection = None
    frameno = 0
    pixeldata = None

    def __init__(self, filepath=None):
        if filepath:
            self.read_panelcfg(filepath)

    def send_swap(self):
        """Shorthand to connection.send_swap"""
        self.connection.send_swap(self.frameno)

    def send_frame(self):
        """Do frame update as suggested in main README, swap previous frame
           and send next frame pixels, use send_swap() to force swap after this
           if you're not sending animations, just make sure to wait a moment
           so the pixeldata has time to arrive"""
        self.connection.send_swap(self.frameno)
        for panel in self.panels:
            panel.send_pixels(self.frameno)
        self.frameno += 1

    def read_panelcfg(self, filepath):
        """Read the config file and init connection + panels"""
        def split_line(line):
            startx, starty, endx, endy, ipaddr, port = line.split(',')
            return (int(startx), int(starty),
                    int(endx), int(endy),
                    ipaddr.strip(), int(port))

        self.panels = []
        with open(filepath, 'rt') as filepntr:
            lines = filepntr.readlines()
            global_config = split_line(lines[0])
            self.connection = Connection(
                global_config[ConfigParts.ip],
                global_config[ConfigParts.port]
            )
            self.size = (
                global_config[ConfigParts.endx],
                global_config[ConfigParts.endy]
            )
            self.pixeldata = get_pixbuf(self.size)
            # Init the panels
            for line in lines[1:]:
                line = line.strip()
                if not line:
                    continue
                if line.startswith('#'):
                    continue
                local_config = split_line(line)
                panel_size = (
                    local_config[ConfigParts.endx] - local_config[ConfigParts.startx],
                    local_config[ConfigParts.endy] - local_config[ConfigParts.starty],
                )
                panel = Panel(
                    local_config[ConfigParts.ip],
                    local_config[ConfigParts.port],
                    p_array=self, size=panel_size,
                    array_position=(
                        local_config[ConfigParts.startx],
                        local_config[ConfigParts.starty]
                    )
                )
                self.panels.append(panel)
