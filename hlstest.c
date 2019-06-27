#include <stdio.h>
#include <stdint.h>
#include "SDL.h"
#include "udpcomm_c.h"

hlsudpcomm_t* ctx;

int scale = 8;
int screensiz_w = 2*64;
int screensiz_h = 1*48;

int sendframenum = 0;
void sendframe(SDL_Surface* image)
{
	if (!ctx)
		return;

	uint16_t data[16*16*3];

#if 1
	for (int yb = 0; yb < screensiz_h; yb += 16)
	{
		for (int xb = 0; xb < screensiz_w; xb += 16)
		{
			for (int xx = 0; xx < 16; xx++)
				for (int yy = 0; yy < 16; yy++)
				{
					int idx = (yy*16+xx)*3;
					int x = xb+xx;
					int y = yb+yy;

					uint32_t *pixels = (uint32_t *)(image->pixels + y * image->pitch);

					int r = (pixels[x] >> 0) & 255;
					int g = (pixels[x] >> 8) & 255;
					int b = (pixels[x] >> 16) & 255;

					data[idx+0] = r*256+r;
					data[idx+1] = g*256+g;
					data[idx+2] = b*256+b;
 
				}


				hlsudp_sendtile(ctx, (uint8_t*)data, 6, sendframenum,xb, yb);
		}
	}
#endif

	hlsudp_sendswap(ctx, sendframenum);
	sendframenum++;

}



int main(int argc, char *argv[])
{
    ctx = hlsudp_open();
    printf("ctx:  %p\n", ctx);


	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window *window = SDL_CreateWindow("base", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screensiz_w*scale, screensiz_h*scale, 0);

	SDL_Surface *screen = SDL_GetWindowSurface(window);

	SDL_Surface *image = SDL_CreateRGBSurface(0, screensiz_w, screensiz_h, 32, 0,0,0,0);
	uint16_t HDRimage[screensiz_w*screensiz_h*3];

	int lasttime = 0;
	int frames = 0;

	while (1)
	{
		SDL_Event event;
		SDL_PollEvent(&event);
		if (event.type == SDL_QUIT) {
			break;
		}
		else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_q)
		{
			break;
		}

		const Uint8* 	keystates = SDL_GetKeyboardState(NULL);

		static int alasttime = 0;
		while((SDL_GetTicks() - alasttime) < 16)
			continue;
		alasttime = SDL_GetTicks();

		int aika = SDL_GetTicks();
		frames++;

		if ((aika-lasttime) > 1000.f)
		{
			float fps = (float)frames / ((float)(aika-lasttime) / 1000.f);
			lasttime = aika;
			frames = 0;
			printf("fps: %.2f\n", fps);
		}


		static int aaika = 0;

		if (!keystates[SDL_SCANCODE_SPACE])
			aaika = aika;



#if 0
		//#pragma omp parallel for schedule(dynamic, 8)
		for (int y = 0; y < screensiz_h; y++)
		{
			uint32_t *pixels = (uint32_t *)(image->pixels + y * image->pitch);

			for (int x = 0; x < screensiz_w; x++)
			{
				pixels[x] = (x*((y+aaika/8)%90));
			}

		}
#endif
		SDL_FillRect(image, NULL, 0x404040);
		{
			float xx = sin(aaika*0.00192f);
			float yy = cos(aaika*0.00553f);
			xx += sin(aaika*0.00435f);
			yy += sin(aaika*0.00336f);
			xx += cos(aaika*0.00153f);
			yy += cos(aaika*0.00151f);

			xx *= screensiz_w/6;
			yy *= screensiz_h/6;

			SDL_Rect rect;

			rect.x = screensiz_w/2 - 20 + xx/2;
			rect.y = screensiz_h/2 - 20 + yy/2;
			rect.w = 40;
			rect.h = 40;
			SDL_FillRect(image, &rect, 0x202020);

			rect.x = screensiz_w/2 - 18 + xx;
			rect.y = screensiz_h/2 - 18 + yy;
			rect.w = 36;
			rect.h = 36;
			SDL_FillRect(image, &rect, 0x0);

			rect.x = screensiz_w/2 - 16 + xx;
			rect.y = screensiz_h/2 - 16 + yy;
			rect.w = 32;
			rect.h = 32;
			SDL_FillRect(image, &rect, rand());
		}

		sendframe(image);

		SDL_BlitScaled(image, NULL, screen, NULL);

		SDL_UpdateWindowSurface(window);

	}

	if (ctx)
		hlsudp_shutdown(ctx);

	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
