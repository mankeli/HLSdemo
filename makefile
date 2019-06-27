hlstest: hlstest.c
	gcc $< -o $@ $(shell sdl2-config --libs --cflags) -lm

