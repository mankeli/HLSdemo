# Hacklab LED System library and demo

## Terminology:
    hls    = Hacklab Led System
	panel  = Single physical LED display device that has it's own IP address.
	screen = Bigger display that is formed from multiple panels.

## Basic usage:

	initialization:
		The lib is initialized with "hlsudpcomm_t* hlsudp_open(void)""
		this initializes send socket, and loads panel definitions
		from a file specified in env variable PANELCONFIG.

	Data sending:
		"void hlsudp_sendtile(hlsudpcomm_t* ctx, uint8_t *pixels, int pixelsize, int frame, int xo, int yo)"
		Provide pointer to context got from hlsudp_open(), 16*16 block of pixel data, pixel size in bytes
		(only 6 is supported, 16 bits per channel RGB), frame number, and x,y coordinates to the
		top-left corner of the tile position on screen.

		After every tile has been sent, you call "void hlsudp_sendswap(hlsudpcomm_t* ctx, int frame)".
		This sends broadcast packet to network which causes all panels to show new image syncronously.

	Closing:
		Use "void hlsudp_shutdown(hlsudpcomm_t* ctx)".


## Panel config file:
	Basic example, 1x2 64x48 panels forming a 64x96 screen.

	0,  0,  64, 96, 192.168.10.255, 9999
	0,  0,  64, 48, 192.168.10.20, 9999
	0, 48,  64, 48, 192.168.10.21, 9999

	Fields within lines are comma separated:
	x origin, y origin, width, height, IP address, UDP port (9999 by default)

	First line defines the screen. It begins from top-left corner 0,0 and is
	64x96 pixels in size. IP address must be the network's broadcast address.

	Subsequent lines define invidual panels. first 64x48 panel is located at
	screen top-left corner 0,0. The Second is under it, and thus begins from coordinates
	0,48.

## HLS setup procedure
	1. Connect all panels to a network with DHCP server
	2. Wait until each panel gets an IP address. It will be shown on the panel
	3. write panel addresses into the config file
	4. enjoy
