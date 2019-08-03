import time

import numpy as np

import hlslib

COLOR_MAX = 2**16 - 1
RED = np.array((COLOR_MAX, 0, 0), dtype=np.uint16)
GREEN = np.array((0, COLOR_MAX, 0), dtype=np.uint16)
BLUE = np.array((0, 0, COLOR_MAX), dtype=np.uint16)

a = hlslib.PanelArray('panelconfig.txt')
# remember numpy and y,x addressing

a.pixeldata[-30:, -30:] = BLUE
a.pixeldata[-60:-30, -60:-30] = GREEN
a.pixeldata[-90:-60, -90:-60] = RED
a.pixeldata[:30, :30] = BLUE
a.pixeldata[30:60, 30:60] = GREEN
a.pixeldata[60:90, 60:90] = RED
a.send_frame()
while True:
    time.sleep(1.0 / 30)
    a.send_frame()
