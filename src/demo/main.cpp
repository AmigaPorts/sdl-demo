// Marlon Beijer, Demo SDL-1.2 Test


#include <iostream>
#include <math.h>
#include <string>
#ifdef WIN32
#pragma comment(lib, "SDLmain.lib")
#pragma comment(lib, "SDL.lib")
#endif

#if _MSC_VER >= 1400
extern "C" _CRTIMP extern FILE _iob[];
extern "C" { FILE __iob_func[3] = { *stdin,*stdout,*stderr }; }
#endif
#include <SDL.h>
#if defined(WIN32)
#undef main
#endif
#include "timer.h"
#include "star_bmp.h"
#include "hex2surface.h"
#include "image.h"
#include "ball.h"
#include "font-16x16-1520x16.h"
#include "stars.h"
#include "moddata.h"
#include "xmp.h"
#ifdef __OSX__
#include "CoreFoundation/CoreFoundation.h"
#endif

using namespace std;

const static int WIDTH = 640;
const static int HEIGHT = 400;
const static int SAMPLERATE = 44100;
const static double SCROLLER_SPEED = 3.67;
const static int SCROLLER_TEXT_HEIGHT = 32;
const static int SCROLLER_Y_TOP = (HEIGHT / 2) - (SCROLLER_TEXT_HEIGHT / 2);
const static string SCROLLER_TEXT[] = { "EEVULNET", "FOREVER LOVING THE AMIGA" };
const static int SCROLLER_TEXT_LEN = sizeof(SCROLLER_TEXT) / sizeof(SCROLLER_TEXT[0]);
const static int FRAMES_PER_SECOND = 60;
const static int STAR_MAX = WIDTH / 15;
const static int STAR_RND[] = { 1, 2, 4, 6 };
static SDL_Surface *font2 = Hex2Surface(font16x16, 1520, 16);
static SDL_Surface *screen;

const static int ballrec = 131;

static int pxa1=0; 
static int pxa2=0; 
static int pya1=0; 
static int pya2=0; 

const static int PILEN = (int)(360*2); 

const static int RX1 = ((WIDTH-16)/4); //76 
const static int RX2 = ((WIDTH-16)/4); //76 
const static int RY1 = ((HEIGHT-16)/4); //44 
const static int RY2 = ((HEIGHT-16)/4); //44 

static long cosTab[PILEN]; 
static long sinTab[PILEN];

static StarBmp STAR_BMP[STAR_MAX];

static int frame = 0;

Timer fps;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static void star_randomize()
{
	for ( int i = 0; i < PILEN; i++ )
	{
		double a = (i * 2 * 3.1415926) * (1.00f / PILEN);
	  	cosTab[i] = (long)(32767 * cos(a));
 		sinTab[i] = (long)(32767 * sin(a));
	} 

	for (int i = 0; i < STAR_MAX; ++i)
	{
		StarBmp val;

		val.x = (int)floor(rand() % WIDTH);
		val.y = (int)floor(rand() % rand() % ( SCROLLER_Y_TOP / 2 ) );
		int rnd = (int)floor(rand() % 4 );

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

		val.bmp = 5 * (int)floor(rand() % 11);

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

	atexit( SDL_Quit );
	return 0;
}

static void blit ( SDL_Surface* image, SDL_Surface* screen, Sint16 srcX, Sint16 srcY, Sint16 dstX, Sint16 dstY, Uint16 width, Uint16 height )
{
	SDL_Rect src = { srcX, srcY, width, height };
	SDL_Rect dest = { dstX, dstY, width, height };

	SDL_BlitSurface( image, &src, screen, &dest );
}

static void DrawText(SDL_Surface* font, SDL_Surface* screen, int posX, int posY, int fontW, int fontH, string text)
{
	int i = 0;

	for ( const char& c : text )
	{
		int fnt = c - 32;

		if ( ( fontW * i ) < WIDTH  && ( fontW * i ) > -16 )
		{
			blit( font, screen, fontW*fnt, 0, (fontW*i) + posX, posY, fontW, fontH );
		}

		i++;
	}
}

static void DrawStars(SDL_Surface* stars, SDL_Surface* screen)
{
	for (int i = 0; i < STAR_MAX; ++i)
	{
		int len = STAR_BMP[i].x + STAR_BMP[i].spd;
		if ( len >= WIDTH ) len = 0;
		STAR_BMP[i].x = len;

		blit( stars, screen, STAR_BMP[i].bmp, 0, len, STAR_BMP[i].y, 5, 5 );
		blit( stars, screen, STAR_BMP[i].bmp, 0, len, STAR_BMP[i].y + (int)( SCROLLER_Y_TOP * 1.5 ), 5, 5 );
	}
}

static int mod( int v, int m )
{ 
	while ( v < 0 ) v += m;

	return v % m; 
} 

static void DrawBalls(SDL_Surface* ball, SDL_Surface* screen)
{
	int pxb1 = pxa1; 
	int pxb2 = pxa2; 
	int pyb1 = pya1; 
	int pyb2 = pya2; 

	for ( int i = 0; i < ballrec; i++ )
	{	
		int x = ((RX1 * cosTab[mod( pxb1, PILEN )]) + (RX2 * sinTab[mod( pxb2, PILEN )])) >> 15; 
     	int y = (RY1 * cosTab[mod( pyb1, PILEN )] + RY2 * sinTab[mod( pyb2, PILEN )]) >> 15; 

     	x += (WIDTH-16)/2; 
     	y += (HEIGHT-16)/2; 

	  	blit( ball, screen, 0,0, x, y, 16, 16 );
			
		pxb1 += 7*2; 
    	pxb2 -= 4*2; 
    	pyb1 += 6*2; 
    	pyb2 -= 3*2; 		
	}	
	
  	pxa1 += 3*2; 
  	pxa2 += 2*2; 
  	pya1 += -1*2; 
  	pya2 += 2*2; 

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

	screen = SDL_SetVideoMode( WIDTH, HEIGHT, 16, SDL_HWSURFACE/* | SDL_FULLSCREEN*/);

	font2 = SDL_DisplayFormat( font2 );
	SDL_SetColorKey( font2, SDL_SRCCOLORKEY, SDL_MapRGB( font2->format, 255, 0, 255) );

	if ( screen == NULL )
	{
		exit( 0 );
	}

	star_randomize();

	SDL_Surface *font = Hex2Surface(image, 864, 32);
	
	SDL_Surface *ball = Hex2Surface(hexball, 16, 16);
	SDL_Surface *stars = Hex2Surface(hexstars, 55, 5);

	font = SDL_DisplayFormat(font);

	SDL_SetColorKey( font, SDL_SRCCOLORKEY, SDL_MapRGB( font->format, 255, 0, 255) );
	

	SDL_Event event;

	bool gameRunning = true;
	int scr = -WIDTH;
	int sin_cnt = 0;
	int textCnt = 0;
	int scnCnt = 2;
	const int sin_pos[] = {4,4,5,6,7,8,10,12,14,16,19,22,25,28,32,36,40,44,49,54,59,65,71,77,83,89,94,99,104,109,113,117,121,125,128,131,134,137,139,141,143,145,146,147,148,149,149,149,149,148,147,146,145,143,141,139,137,134,131,128,125,121,117,113,109,104,99,94,89,83,77,71,65,59,54,49,44,40,36,32,28,25,22,19,16,14,12,10,8,7,6,5,4,4};
	const int sin_len = sizeof(sin_pos)/sizeof(sin_pos[0]);

	char *tune = (char *)"blitz.mod";

	if (xmp_load_module_from_memory(ctx, (void*)moddata, moddatalen) < 0)
	{
		fprintf(stderr, "%s: error loading %s\n", argv[0], tune);
		exit(0);
	}

	xmp_start_player(ctx, SAMPLERATE, 0);
	SDL_PauseAudio(1);

	while (gameRunning)
	{
		//Start the frame timer
		fps.start();

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
						if (scnCnt == 0)
						{
							scnCnt = 1;
							scr = -WIDTH;
							SDL_PauseAudio(0);
						}
						break;
					default:
						break;
				}
			}
		}

		SDL_FillRect( screen, NULL, SDL_MapRGB( screen->format, 0, 0, 0 ) );

		int i = 0;
		if ( ++sin_cnt >= sin_len ) sin_cnt = 0;

		DrawStars(stars, screen);

		SDL_Rect box = { 0, SCROLLER_Y_TOP / 2, WIDTH, 1 };
		SDL_FillRect( screen, &box, 1500 );
		box.y = SCROLLER_Y_TOP + ( SCROLLER_Y_TOP / 2 );
		SDL_FillRect( screen, &box, 1500 );
		box.y = ( SCROLLER_Y_TOP / 2 ) + 1;
		box.h = ( SCROLLER_Y_TOP ) - 1;
		SDL_FillRect( screen, &box, 0xFF0000 );

		

		if (scnCnt == 0)
		{
			scr = -((WIDTH / 2) - 8 * 16);
			for ( const char& c : SCROLLER_TEXT[textCnt] )
			{
				int fnt = c - 64;
				int offsetY = 0;

				if ( (32*i) - scr < WIDTH  && (32*i) - scr > -32 )
				{
					blit( font, screen, 32*fnt, 0, (32*i)-scr, SCROLLER_Y_TOP + offsetY - 16, 32, 32 );
				}

				i++;
			}

			DrawText(font2, screen, (WIDTH/2)-((16*11)/2), SCROLLER_Y_TOP+32, 16, 16, "PRESS ENTER");

		}
		else if (scnCnt == 1)
		{
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

			scr = scr + (int)SCROLLER_SPEED;
			if ( scr > SCROLLER_TEXT[textCnt].size()*32 )
			{
				scr = -WIDTH;
				if (++textCnt >= SCROLLER_TEXT_LEN)
				{
					textCnt = 0;
					++scnCnt;
				}
			}
		}
		else if (scnCnt == 2)
		{
			DrawBalls(ball,screen);
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
	SDL_FreeSurface(ball);
	SDL_FreeSurface(stars);
	SDL_FreeSurface(font);
	SDL_FreeSurface(font2);

	SDL_Quit();

	return 0;
}
