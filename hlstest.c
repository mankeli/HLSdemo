#include <stdio.h>
#include <stdint.h>
#include "SDL.h"
#include "SDL_ttf.h"
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

					int r = (pixels[x] >> 16) & 255;
					int g = (pixels[x] >> 8) & 255;
					int b = (pixels[x] >> 0) & 255;

					data[idx+0] = r*r;
					data[idx+1] = g*g;
					data[idx+2] = b*b;
				}


				hlsudp_sendtile(ctx, (uint8_t*)data, 6, sendframenum,xb, yb);
		}
	}
#endif

	hlsudp_sendswap(ctx, sendframenum);
	sendframenum++;

}


TTF_Font* loadfont(char* file, int ptsize) {
	TTF_Font* tmpfont;
	tmpfont = TTF_OpenFont(file, ptsize);
	if (tmpfont == NULL){
	printf("Unable to load font: %s %s \n", file, TTF_GetError());
	// Handle the error here.
	}
	return tmpfont;
}

void drawtext(SDL_Surface* image, TTF_Font *font, int x, int y, const char* text, uint32_t color)
{
	SDL_Color tmpfontcolor = *(SDL_Color*)(&color);
	SDL_Surface *resulting_text;

	resulting_text = TTF_RenderText_Blended(font, text, tmpfontcolor);

		SDL_Rect rect;
		rect.x = x;
		rect.y = y;
		rect.w = resulting_text->w;
		rect.h = resulting_text->h;
	SDL_BlitSurface(resulting_text, NULL, image, &rect);

	SDL_FreeSurface(resulting_text);
}



int min(int v1, int v2)
{
	return v1 < v2 ? v1 : v2;
}

int max(int v1, int v2)
{
	return v1 > v2 ? v1 : v2;
}

void gradientrect(SDL_Surface* image, SDL_Rect* rect)
{
	int x1 = max(rect->x, 0);
	int x2 = min(rect->x + rect->w, image->w);
	int y1 = max(rect->y, 0);
	int y2 = min(rect->y + rect->h, image->h);

	int rd = 0xc000 / image->h;
	int gd = 0x6000 / image->h;
	int bd = 0x5000 / image->h;
	int ra = 0x4000 + rd * y1;
	int ga = 0x8000 + gd * y1;
	int ba = 0x3000 + bd * y1;


	for (int y = y1; y < y2; y++)
	{
		uint32_t *pixels = (uint32_t *)(image->pixels + y * image->pitch);

		for (int x = x1; x < x2; x++)
		{
			int r8 = (ra / 256) & 0xFF;
			int g8 = (ga / 256) & 0xFF;
			int b8 = (ba / 256) & 0xFF;
			pixels[x] = (r8 << 16) | (g8 << 8) | (b8);
		}

		ra += rd;
		ga += gd;
		ba += bd;

	}
}

void draweffy(SDL_Surface* image)
{
	static float aaika;

	const Uint8* keystates = SDL_GetKeyboardState(NULL);
	if (!keystates[SDL_SCANCODE_SPACE])
		aaika = SDL_GetTicks();

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
		//SDL_FillRect(image, &rect, rand());
		gradientrect(image, &rect);
	}
}

int main(int argc, char *argv[])
{
	// open HLSUDP context
    ctx = hlsudp_open();
    printf("ctx:  %p\n", ctx);


    // sdl init crap
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window *window = SDL_CreateWindow("base", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screensiz_w*scale, screensiz_h*scale, 0);

	SDL_Surface *screen = SDL_GetWindowSurface(window);

	SDL_Surface *image = SDL_CreateRGBSurface(0, screensiz_w, screensiz_h, 32, 0,0,0,0);

	if (TTF_Init() == -1) {
		printf("Unable to initialize SDL_ttf: %s \n", TTF_GetError());
		return 1;
	}

	TTF_Font* font = loadfont("LeroyLetteringLightBeta01.ttf", 20);
	TTF_Font* font2 = loadfont("LeroyLetteringLightBeta01.ttf", 15);

	int lasttime = 0;
	int frames = 0;

	while (1)
	{
		// read events etc
		SDL_Event event;
		SDL_PollEvent(&event);
		if (event.type == SDL_QUIT) {
			break;
		}
		else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_q)
		{
			break;
		}


		// wait atleast 16msec between frames ~= 60fps
		#define FRAMETIME 16
		static int alasttime = 0;
		while((SDL_GetTicks() - alasttime) < FRAMETIME)
			continue;
		alasttime = SDL_GetTicks();


		int aika = SDL_GetTicks();
		frames++;

		// print fps meter every second
		if ((aika-lasttime) > 1000.f)
		{
			float fps = (float)frames / ((float)(aika-lasttime) / 1000.f);
			lasttime = aika;
			frames = 0;
			printf("fps: %.2f\n", fps);
		}

		// draw effy
		draweffy(image);

		drawtext(image, font2, 5, 5, "Hacklab", 0xFF90FF90);
		drawtext(image, font, 5, 16, "LED", 0xFF9090FF);
		drawtext(image, font, 20, 27, "system", 0xFFFFe0e0);

		// convert image to HDR and send to panels
		sendframe(image);

		// blit to screen for preview
		SDL_BlitScaled(image, NULL, screen, NULL);
		SDL_UpdateWindowSurface(window);

	}

	if (ctx)
		hlsudp_shutdown(ctx);

	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
