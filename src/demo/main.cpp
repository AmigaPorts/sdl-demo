// Marlon Beijer, Demo SDL-1.2 Test

#include <SDL.h>
#include <iostream>
#include <math.h>
#include <string>
#include "timer.h"
#include "star_bmp.h"
#include "hex2surface.h"
#include "image.h"
#include "stars.h"
#include "moddata.h"
#include "xmp.h"
#ifdef __OSX__
#include "CoreFoundation/CoreFoundation.h"
#endif

const static int WIDTH = 640;
const static int HEIGHT = 400;
const static int SAMPLERATE = 44100;
const float SCROLLER_SPEED = 3.67;
const int SCROLLER_TEXT_HEIGHT = 32;
const int SCROLLER_Y_TOP = (HEIGHT / 2) - (SCROLLER_TEXT_HEIGHT / 2);
const std::string SCROLLER_TEXT[] = { "HERES A PROOF OF CONCEPT DEMO", "FOR SOME VAMPIRIZED AMIGAS", "MADE BY MARLON BEIJER" };
const int FRAMES_PER_SECOND = 60;
const int STAR_MAX = WIDTH / 15;
const int STAR_RND[] = { 1, 2, 4, 6 };
static StarBmp STAR_BMP[STAR_MAX];

static int frame = 0;

Timer fps;

static void star_randomize()
{
	for (int i = 0; i < STAR_MAX; ++i)
	{
		StarBmp val;

		val.x = floor(rand() % WIDTH);
		val.y = floor(rand() % rand() % ( SCROLLER_Y_TOP / 2 ) );
		int rnd = floor(rand() % 4 );

		val.spd = STAR_RND[rnd];

		if(val.y > 100)
		{
			if(val.spd > 1)
			{
				val.spd -= 1;
			}
			else
			{
				val.y -= 100;
			}
		}

		if(val.y > 150)
		{
			if(val.spd > 1)
			{
				val.spd -= 1;
			}
			else
			{
				val.y -= 150;
			}
		}

		if (val.y > 200)
		{
			if(val.spd > 1)
			{
				val.spd -= 1;
			}
			else
			{
				val.y -= 200;
			}
		}

		if(val.y > 250)
		{
			if(val.spd > 1)
			{
				val.spd -= 1;
			}
			else
			{
				val.y -= 250;
			}
		}

		val.bmp = 5 * floor(rand() % 11);

		STAR_BMP[i] = val;
	}
}

static int playing;

static void fill_audio(void *udata, Uint8 *stream, int len)
{
	if (xmp_play_buffer((xmp_context)udata, stream, len, 0) < 0)
		playing = 0;
}

static int init(xmp_context ctx)
{
	char name[64];

	if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER ) != 0 )
	{
		fprintf(stderr, "Unable to initialize SDL:%s\n", SDL_GetError());
		return -1;
	}

	SDL_AudioSpec a;

	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		fprintf(stderr, "SDL: can't initialize: %s\n", SDL_GetError());
		return -1;
	}

	a.freq = SAMPLERATE;
	a.format = AUDIO_S16SYS;
	a.channels = 2;
	a.samples = 2048;
	a.callback = fill_audio;
	a.userdata = ctx;

	if (SDL_OpenAudio(&a, NULL) < 0) {
		fprintf(stderr, "%s\n", SDL_GetError());
		return -1;
	}

	//printf("SDL: Using audio driver: %s\n", SDL_AudioDriverName(name, 32));

	atexit( SDL_Quit );
	return 0;
}

static void blit ( SDL_Surface* image, SDL_Surface* screen, Sint16 srcX, Sint16 srcY, Sint16 dstX, Sint16 dstY, Uint16 width, Uint16 height )
{
	SDL_Rect src = { srcX, srcY, width, height };
	SDL_Rect dest = { dstX, dstY, width, height };

	SDL_BlitSurface( image, &src, screen, &dest );
}

#if defined(__OSX__)
extern "C" int SDL_main(int argc, char *argv[])
#else
int main ( int argc, const char* argv[] )
#endif
{
#ifdef __OSX__
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
    char path[PATH_MAX];
    if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, PATH_MAX)) // Error: expected unqualified-id before 'if'
    {
        // error!
    }
    CFRelease(resourcesURL); // error: expected constructor, destructor or type conversion before '(' token

    chdir(path); // error: expected constructor, destructor or type conversion before '(' token
    std::cout << "Current Path: " << path << std::endl; // error: expected constructor, destructor or type conversion before '<<' token
#endif

	xmp_context ctx;
	ctx = xmp_create_context();

	if (init(ctx) < 0)
	{
		fprintf(stderr, "%s: can't initialize sound\n", argv[0]);
		exit(1);
	}

	SDL_ShowCursor(0);

	SDL_Surface *screen = SDL_SetVideoMode( WIDTH, HEIGHT, 16, SDL_HWSURFACE | SDL_FULLSCREEN );

	if ( screen == NULL )
	{
		exit( 0 );
	}

	star_randomize();

	float deltaTime = 0.0;
	int thisTime = 0;
	int lastTime = 0;

	SDL_Surface *fontImg = Hex2Surface(image, 864, 32);
	SDL_Surface *stars = Hex2Surface(hexstars, 55, 5);

	SDL_Surface *font = SDL_DisplayFormat(fontImg);
	SDL_FreeSurface( fontImg );

	SDL_SetColorKey( font, SDL_SRCCOLORKEY, SDL_MapRGB( font->format, 255, 0, 255) );

	SDL_Event event;

	bool gameRunning = true;
	float scr = -WIDTH;
	int sin_cnt = 0;
	int textCnt = 0;
	const int sin_pos[] = {4,4,5,6,7,8,10,12,14,16,19,22,25,28,32,36,40,44,49,54,59,65,71,77,83,89,94,99,104,109,113,117,121,125,128,131,134,137,139,141,143,145,146,147,148,149,149,149,149,148,147,146,145,143,141,139,137,134,131,128,125,121,117,113,109,104,99,94,89,83,77,71,65,59,54,49,44,40,36,32,28,25,22,19,16,14,12,10,8,7,6,5,4,4};
	const int sin_len = sizeof(sin_pos)/sizeof(sin_pos[0]);

	char *tune = (char *)"blitz.mod";

	if (xmp_load_module_from_memory(ctx, (void*)moddata, moddatalen) < 0)
	{
		fprintf(stderr, "%s: error loading %s\n", argv[0], tune);
		exit(0);
	}

	xmp_start_player(ctx, SAMPLERATE, 0);
	SDL_PauseAudio(0);

	while (gameRunning)
	{
		//Start the frame timer
		fps.start();

		thisTime = SDL_GetTicks();
		deltaTime = (float)(thisTime - lastTime) / 1000;
		lastTime = thisTime;

		if (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				gameRunning = false;
			}

			if (event.type == SDL_KEYDOWN)
			{
				SDLKey keyPressed = event.key.keysym.sym;
				switch (keyPressed)
				{
					case SDLK_ESCAPE:
						gameRunning = false;
						break;
					case SDLK_RETURN:
						break;
					default:
						break;
				}
			}
		}

		SDL_FillRect( screen, NULL, SDL_MapRGB( screen->format, 0, 0, 0 ) );

		int i = 0;
		if ( ++sin_cnt >= sin_len ) sin_cnt = 0;

		for (int i = 0; i < STAR_MAX; ++i)
		{
			int len = STAR_BMP[i].x + STAR_BMP[i].spd;
			if ( len >= WIDTH ) len = 0;
			STAR_BMP[i].x = len;

			blit( stars, screen, STAR_BMP[i].bmp, 0, len, STAR_BMP[i].y, 5, 5 );
			blit( stars, screen, STAR_BMP[i].bmp, 0, len, STAR_BMP[i].y + ( SCROLLER_Y_TOP * 1.5 ), 5, 5 );
		}

		SDL_Rect box = { 0, SCROLLER_Y_TOP / 2, WIDTH, 1 };
		SDL_FillRect( screen, &box, 1500 );
		box.y = SCROLLER_Y_TOP + ( SCROLLER_Y_TOP / 2 );
		SDL_FillRect( screen, &box, 1500 );
		box.y = ( SCROLLER_Y_TOP / 2 ) + 1;
		box.h = ( SCROLLER_Y_TOP ) - 1;
		SDL_FillRect( screen, &box, 0xFF0000 );

		for ( const char& c : SCROLLER_TEXT[textCnt] )
		{
			int fnt = c - 64;
			int offsetY = 0;

			offsetY = sin_cnt + i;

			if ( offsetY >= sin_len )
				offsetY -= sin_len;

			offsetY = sin_pos[offsetY] - (149/2);

			if ( (32*i) - scr < WIDTH  && (32*i) - scr > -32 )
			{
				blit( font, screen, 32*fnt, 0, (32*i)-scr, SCROLLER_Y_TOP + offsetY - 16, 32, 32 );
			}


			i++;
		}


		scr = scr + SCROLLER_SPEED;// * deltaTime);
		if ( scr > SCROLLER_TEXT[textCnt].size()*32 )
		{
			scr = -WIDTH;
			if (++textCnt >= sizeof(SCROLLER_TEXT) / sizeof(SCROLLER_TEXT[0]) ) textCnt = 0;
		}

		SDL_Flip(screen);
		frame++;

		if( fps.get_ticks() < 1000 / FRAMES_PER_SECOND )
		{
			SDL_Delay( ( 1000 / FRAMES_PER_SECOND ) - fps.get_ticks() );
		}

	}

	xmp_end_player(ctx);
	xmp_release_module(ctx);
	xmp_free_context(ctx);

	SDL_CloseAudio();
	SDL_FreeSurface(stars);
	SDL_FreeSurface(font);

	SDL_Quit();

	return 0;
}
