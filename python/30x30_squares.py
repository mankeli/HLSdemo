import hlslib
import time
import numpy as np

COLOR_MAX = 2**16-1

a = hlslib.PanelArray('panelconfig.txt')
# remember numpy and y,x addressing

a.pixeldata[-30:,-30:]=np.array((COLOR_MAX, 0, 0), dtype=np.uint16)
a.pixeldata[-60:-30,-60:-30]=np.array((0, COLOR_MAX, 0), dtype=np.uint16)
a.pixeldata[-90:-60,-90:-60]=np.array((0, 0, COLOR_MAX), dtype=np.uint16)
a.pixeldata[:30,:30]=np.array((COLOR_MAX, 0, 0), dtype=np.uint16)
a.pixeldata[30:60,30:60]=np.array((0, COLOR_MAX, 0), dtype=np.uint16)
a.pixeldata[60:90,60:90]=np.array((0, 0, COLOR_MAX), dtype=np.uint16)
a.send_frame()
while True:
    time.sleep(1.0/30)
    a.send_frame()
