#include <proto/exec.h>
#include <exec/exec.h>
#include <stdio.h>
#include <timer.h>
#include <palette.h>
#include <canvas.h>
//#include <font.h>
//#include <ball.h>
#include <cstdlib>
#include <clib/graphics_protos.h>
#include "video.h"
//#include "pics1.h"
//#include "pics1_palette.h"
//#include "hex2surface.h"
//#include "tiletypes.h"
//#undef exit

#define TileIndex(x, y, tileWidth, tilesetHeight) (x / tileWidth * tilesetHeight + x % tileWidth + y * tileWidth)
#define TileX(levelTile, tileWidth, tilesetHeight)  (( ( levelTile / tilesetHeight ) * tileWidth + levelTile % tileWidth ) * tileWidth)
#define TileY(levelTile, tileWidth, tilesetHeight)  (( ( levelTile % tilesetHeight  ) / tileWidth ) * tileWidth)

#ifdef __AMIGA__
#define WIDTH 320
#define HEIGHT 200
#else
#define WIDTH 320
#define HEIGHT 200
#endif
const static int SAMPLERATE = 44100;
const static double SCROLLER_SPEED = 3.67;
const static int SCROLLER_TEXT_HEIGHT = 16;
const static int SCROLLER_Y_TOP = (HEIGHT / 2) - (SCROLLER_TEXT_HEIGHT / 2);

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

	SDL_Init(0);
	auto * screen = SDL_SetVideoMode(320, 200, 8, 0);

	atexit(SDL_Quit);
	bool gameRunning = true;

	SDL_Rect srcrect = { 0, 0, 8, 8 };
	SDL_Rect rect = { 0, 0, 50, 50 };
	SDL_Rect clear = { 0, 0, 320, 200 };
	SDL_Rect tileDest = {0,0,8,8};
	SDL_Rect tile = {0,0,8,8};

	//SDL_Surface *pics1 = Hex2Surface(hexpics1, hexpics1_width, hexpics1_height);
	//SDL_Surface *ball = Hex2Surface(hexball, 8, 8);
	struct BitMap ballPlanar = {8, 8, 0, 2, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};;
	/*ballPlanar.Planes[0] = {
			0,0,0,0,0,0,0,0,
			0,1,1,1,1,1,1,0,
			0,1,0,0,0,0,1,0,
			0,1,0,0,0,0,1,0,
			0,1,0,0,0,0,1,0,
			0,1,0,0,0,0,1,0,
			0,1,1,1,1,1,1,0,
			0,0,0,0,0,0,0,0,
	};
	ballPlanar.Planes[1] = {
			0,0,0,0,0,0,0,0,
			0,0,1,1,1,1,1,0,
			0,1,2,0,0,0,1,0,
			0,1,0,0,0,0,1,0,
			0,1,0,0,0,0,1,0,
			0,1,0,0,0,0,1,0,
			0,1,1,1,1,1,1,0,
			0,0,0,0,0,0,0,0,
	};*/

	//c2p(ball, &ballPlanar);
	//c2p2(&srcrect, (void *)hexball, ballPlanar);
	//SDL_Surface *tiletypes1 = Hex2Surface(assets_tiletypes1_dat, hexpics1_width/8, hexpics1_height/8);


	SDL_SetPalette(screen, SDL_LOGPAL | SDL_PHYSPAL, colors[0], 0, 256);
	//ADL_SetPalette(hexpics1_palette, 0, 256);
	int levelWidth = 6, levelHeight = 256;
/*	unsigned short level[] = {
			0x000, TileIndex(2,0,8,hexpics1_height), TileIndex(3,0,8,hexpics1_height), 0x3ff, 0x3ff, 0x000,
			0x000, TileIndex(2,1,8,hexpics1_height), TileIndex(3,1,8,hexpics1_height), 0x14a, 0x14a, 0x000,
			0x000, 0x1ff, 0x1ff, 0x1ff, 0x1ff, 0x000,
			0x000, 0x1ff, 0x1ff, 0x1ff, 0x000, 0x000,
			0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
			0x000, 0x1ff, 0x1ff, 0x1ff, 0x000, 0x000,
			0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
			0x000, 0x1ff, 0x1ff, 0x1ff, 0x000, 0x000,
			0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
			0x000, 0x1ff, 0x1ff, 0x1ff, 0x000, 0x000,
			0x000, 0x1ff, 0x1ff, 0x1ff, 0x000, 0x000,
			0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
			0x000, 0x1ff, 0x1ff, 0x1ff, 0x000, 0x000,
			0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
			0x000, 0x1ff, 0x1ff, 0x1ff, 0x000, 0x000,
			0x000, 0x1ff, 0x1ff, 0x1ff, 0x000, 0x000,
			0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
			0x000, 0x1ff, 0x1ff, 0x1ff, 0x000, 0x000,
			0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
			0x000, 0x1ff, 0x1ff, 0x1ff, 0x000, 0x000,
			0x000, 0x1ff, 0x1ff, 0x1ff, 0x000, 0x000,
			0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
			0x000, 0x1ff, 0x1ff, 0x1ff, 0x000, 0x000,
			0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
			0x000, 0x1ff, 0x1ff, 0x1ff, 0x000, 0x000,
	};
*/
	int x=0, y=0, modx = 1, mody =1;

	while(gameRunning) {
		tile.w = tile.h = tileDest.w = tileDest.h = 8;
		if (x >= WIDTH-10) modx = -1;
		if (y >= HEIGHT-10) mody = -1;
		if (x < 2) modx = 1;
		if (y < 2) mody = 1;

		//printf("draw");

		switch (getKey()) {
			case SDLK_ESCAPE:
			case SDLK_RETURN:
				gameRunning = false;
				printf("keyPressed: %04x", getKey());
				break;
			default:

				break;
		}

		//ADL_FillRect(&clear, 7);

		//DrawStars(stars, screen);
		SDL_Rect box = { 0, SCROLLER_Y_TOP / 2, WIDTH, 1 };
		uint32_t color = 2;
		ADL_FillRect(&box, color);
		box.y = SCROLLER_Y_TOP + (SCROLLER_Y_TOP / 2);
		ADL_FillRect(&box, color);
		box.y = (SCROLLER_Y_TOP / 2) + 1;
		box.h = (SCROLLER_Y_TOP) - 1;

		//for ( y = 0; y < 3; y++ ) {

			//for ( x = 0; x < levelWidth; x++ ) {

				tileDest.x = x;
				tileDest.y = y;
				tile.x = 0;
				tile.y = 0;

			//	tile.x = TileX(level[y * levelWidth + x], tile.w, hexpics1_height);
				//tile.y = TileY(level[y * levelWidth + x], tile.w, hexpics1_height);

				//if (ballPlanar == nullptr) exit(1);
				//ADL_BlitBitPlane(&ballPlanar, &srcrect, &tileDest);

				ADL_FillRect(&tileDest, y);
				printf("tile X(%d),Y(%d),TileId(%d),TileType(%d)\n", tile.x, tile.y, level[y * levelWidth + x], assets_tiletypes1_dat[level[y * levelWidth + x]]);
				//SDL_BlitSurface(ball, &tile, screen, &tileDest);
			//}
		//}
		y += mody;
		x += modx;


		SDL_Delay(500);
		//SDL_BlitSurface(pics1, &tile, screen, &tileDest);

		//ADL_FillRect(&rect, 5);
		//SDL_Flip(screen);


		//exit(0);

	}


	exit(0);
}
