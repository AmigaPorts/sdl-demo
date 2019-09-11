
#include <proto/exec.h>
#include <exec/exec.h>
#include <stdio.h>
#include <timer.h>
#include <palette.h>
#include <canvas.h>
#include <vga.h>
#include <font.h>
#include <ball.h>
#include "video.h"

#undef exit

int main(int argc, char *argv[]) {
	int cpu_type = 0;
	// find out what type of CPU we have

	if ((SysBase->AttnFlags & AFF_68060) != 0 )
		cpu_type = 68060;
	else if ((SysBase->AttnFlags & AFF_68040) != 0 )
		cpu_type = 68040;
	else if ((SysBase->AttnFlags & AFF_68030) != 0 )
		cpu_type = 68030;
	else if ((SysBase->AttnFlags & AFF_68020) != 0 )
		cpu_type = 68020;
	else if ((SysBase->AttnFlags & AFF_68010) != 0 )
		cpu_type = 68010;
	else
		cpu_type = 68000;

	if ( cpu_type < 68020 ) {
		printf("\nYou need a 68020 or higher to run this engine\n\n");
		return 0;
	}

	SDL_SetVideoMode(320, 200, 8, 0);

	SDL_Surface *test;
	test = (SDL_Surface *)AllocVec(sizeof(*test), MEMF_FAST);
	test->w = 320;
	test->h = 200;
	test->pixels = (void *)hexcanvas;

	SDL_Surface *test2;
	test2 = (SDL_Surface *)AllocVec(sizeof(*test2), MEMF_FAST);
	test2->w = 8;
	test2->h = 8;
	test2->pixels = (void *)hexball;

	SDL_SetPalette(test, SDL_LOGPAL | SDL_PHYSPAL, colors[0], 0, 256);

	SDL_Rect src = { 0, 0, 320, 200 };

	SDL_Rect font = { 0, 0, 8, 8 };

	SDL_FillRect(test, &src, 30);

	src.w = 50;
	src.h = 50;
	src.y = 200 / 2;
	src.x = 320 / 2;

	for ( int i = 0; i < 320; i++ ) {
		SDL_BlitSurface(test2, &font, test, &src);

		SDL_FillRect(test, &src, 5);
		SDL_Flip(test);
		//printf("\nspin: %d\n\n", i);
		sleep(500);
	}

	SDL_Quit();

	return 0;
}
