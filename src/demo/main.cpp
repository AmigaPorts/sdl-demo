// Marlon Beijer, Demo SDL-1.2
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



#ifndef __AMIGA__
#include "xmp.h"
#include <SDL.h>

#if defined(WIN32)
#undef main
#endif
#else
#include "video.h"
#include "Sound.h"
#include "control.h"
#endif
#include "demoTimer.h"
#include "star_bmp.h"
#include "hex2surface.h"
#include "font.h"
#include "ball.h"
#include "stars.h"
#include "moddata.h"
//#include "font-16x16-1520x16.h"

#ifdef __OSX__
#include "CoreFoundation/CoreFoundation.h"
#endif

using namespace std;
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
const static string SCROLLER_TEXT[] = {"EEVULNET", "FOREVER LOVING THE AMIGA"};
const static int SCROLLER_TEXT_LEN = sizeof(SCROLLER_TEXT) / sizeof(SCROLLER_TEXT[0]);
const static int FRAMES_PER_SECOND = 60;
const static int STAR_MAX = WIDTH / 15;
const static int STAR_RND[] = {1, 2, 4, 6};
//static SDL_Surface *font2 = Hex2Surface(font16x16, 1520, 16);
static SDL_Surface *screen;
const static int ballrec =
#ifdef __AMIGA__
		230
#else
		230
#endif
;
static int pxa1 = 0;
static int pxa2 = 0;
static int pya1 = 0;
static int pya2 = 0;

const static int PILEN = (int) (360 * 2);

const static int RX1 = ((WIDTH - 16) / 4); //76
const static int RX2 = ((WIDTH - 16) / 4); //76
const static int RY1 = ((HEIGHT - 16) / 4); //44
const static int RY2 = ((HEIGHT - 16) / 4); //44

static long cosTab[PILEN];
static long sinTab[PILEN];
static long ballX[PILEN][ballrec];
static long ballY[PILEN][ballrec];
static StarBmp STAR_BMP[STAR_MAX];
static int ballCounter = 0;
static int frame = 0;

Timer fps;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static inline int mod(int v, int m) {
	while (v < 0) v += m;

	return v % m;
}

static void star_randomize() {
	for (int i = 0; i < PILEN; i++) {
		double a = (i * 2 * M_PI) * (1.00f / PILEN);
		cosTab[i] = (long) (16383 * cos(a));
		sinTab[i] = (long) (16383 * sin(a));
	}

	int pxb1 = pxa1;
	int pxb2 = pxa2;
	int pyb1 = pya1;
	int pyb2 = pya2;

	for (int i = 0; i < PILEN; i++) {
		pxb1 = pxa1;
		pxb2 = pxa2;
		pyb1 = pya1;
		pyb2 = pya2;

		for (int j = 0; j < ballrec; j++) {
			int x = (((RX1 * cosTab[mod(pxb1, PILEN)]) + (RX2 * sinTab[mod(pxb2, PILEN)])) >> 15) + (WIDTH - 8) / 2;
			int y = ((RY1 * cosTab[mod(pyb1, PILEN)] + RY2 * sinTab[mod(pyb2, PILEN)]) >> 15) + (HEIGHT - 8) / 2;

			ballX[i][j] = x;
			ballY[i][j] = y;
			pxb1 += 4 * 2;
			pxb2 -= 2 * 2;
			pyb1 += 6 * 2;
			pyb2 -= 3 * 2;
		}

		pxa1 += 3 * 2;
		pxa2 += 2 * 2;
		pya1 += -1 * 2;
		pya2 += 2 * 2;

	}

	for (auto &i : STAR_BMP) {
		StarBmp val;

		val.x = (int) floor(rand() % WIDTH);
		val.y = (int) floor(rand() % rand() % (SCROLLER_Y_TOP / 2));
		int rnd = (int) floor(rand() % 4);

		val.spd = STAR_RND[rnd];

		if (val.y > 100) {
			val.spd > 1 ? (val.spd -= 1) : (val.y -= 100);
		}

		if (val.y > 150) {
			val.spd > 1 ? (val.spd -= 1) : (val.y -= 150);
		}

		if (val.y > 200) {
			val.spd > 1 ? (val.spd -= 1) : (val.y -= 200);
		}

		if (val.y > 250) {
			val.spd > 1 ? (val.spd -= 1) : (val.y -= 250);
		}

		val.bmp = 5 * (int) floor(rand() % 11);

		i = val;
	}
}

static int playing;

static void fill_audio(void *udata, uint8_t *stream, int len) {
#ifndef __AMIGA__
	if (xmp_play_buffer((xmp_context) udata, stream, len, 0) < 0)
		playing = 0;
#endif
}

static int init(
#ifndef __AMIGA__
		xmp_context ctx
#endif
) {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
		fprintf(stderr, "Unable to initialize SDL:%s\n", SDL_GetError());
		return -1;
	}
#ifndef __AMIGA__
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
#endif

	atexit(SDL_Quit);

	return 0;
}

static void
blit(SDL_Surface *image, SDL_Surface *screen, int16_t srcX, int16_t srcY, int16_t dstX, int16_t dstY, uint16_t width,
	 uint16_t height) {
	SDL_Rect src = {srcX, srcY, width, height};
	SDL_Rect dest = {dstX, dstY, width, height};
	SDL_BlitSurface(image, &src, screen, &dest);
}

static void DrawText(SDL_Surface *font, SDL_Surface *screen, int posX, int posY, int fontW, int fontH, string text) {
	int i = 0;

	for (const char &c : text) {
		int fnt = c - 65;
		int offsetY = 0;
		int letterX = fontW*fnt - (((fontW * fnt)/320)*320);
		int letterY = (int((fontW * fnt)/320)*fontH);

		if ((fontW * i) < WIDTH && (fontW * i) > -fontW) {
			blit(font, screen, letterX, letterY, (fontW * i) + posX, posY, fontW, fontH);
		}

		i++;
	}
}

static void DrawStars(SDL_Surface *stars, SDL_Surface *screen) {
	for (auto &i : STAR_BMP) {
		int len = i.x + i.spd;
		if (len >= WIDTH) len = 0;
		i.x = len;

		blit(stars, screen, i.bmp, 0, len, i.y, 5, 5);
		blit(stars, screen, i.bmp, 0, len, i.y + (int) (SCROLLER_Y_TOP * 1.5), 5, 5);
	}
}

static void DrawBalls(SDL_Surface *ball, SDL_Surface *screen) {

	for (int i = 0; i < ballrec; i++) {
		blit(ball, screen, 0, 0, ballX[ballCounter][i], ballY[ballCounter][i], 8, 8);
	}
	ballCounter++;

	if (ballCounter == PILEN)
		ballCounter = 0;

}

#if defined(__OSX__)
extern "C" int SDL_main(int argc, char *argv[])
#else

int main(int argc, const char *argv[])
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

#ifndef __AMIGA__
	xmp_context ctx;
	ctx = xmp_create_context();
#endif
	if (init(
#ifndef __AMIGA__
		ctx
#endif
) < 0) {
		fprintf(stderr, "%s: can't initialize sound\n", argv[0]);
		exit(1);
	}

	SDL_ShowCursor(0);

	screen = SDL_SetVideoMode(WIDTH, HEIGHT, 8, SDL_SWSURFACE | SDL_FULLSCREEN | SDL_HWPALETTE);

	if (screen == nullptr) {
		exit(0);
	}

	SDL_SetPalette(screen,SDL_LOGPAL|SDL_PHYSPAL, colors[0], 0, 256);

	star_randomize();

	SDL_Surface *font = Hex2Surface(hexfont, 320, 48);
	SDL_Surface *ball = Hex2Surface(hexball, 8, 8);
	SDL_Surface *stars = Hex2Surface(hexstars, 55, 5);

#ifndef __AMIGA__
	font = SDL_DisplayFormat(font);
	ball = SDL_DisplayFormat(ball);
	stars = SDL_DisplayFormat(stars);
#endif
	SDL_SetColorKey(font, SDL_SRCCOLORKEY, 0);
	SDL_SetColorKey(ball, SDL_SRCCOLORKEY, 0);
	SDL_SetColorKey(stars, SDL_SRCCOLORKEY, 0);

#ifndef __AMIGA
	SDL_Event event;
#endif
	bool gameRunning = true;
	double scr = -WIDTH;
	int sin_cnt = 0;
	int textCnt = 0;
	int scnCnt = 1;
	const int sin_pos[] = {4, 4, 5, 6, 7, 8, 10, 12, 14, 16, 19, 22, 25, 28, 32, 36, 40, 44, 49, 54, 59, 65, 71, 77, 83,
						   89, 94, 99, 104, 109, 113, 117, 121, 125, 128, 131, 134, 137, 139, 141, 143, 145, 146, 147,
						   148, 149, 149, 149, 149, 148, 147, 146, 145, 143, 141, 139, 137, 134, 131, 128, 125, 121,
						   117, 113, 109, 104, 99, 94, 89, 83, 77, 71, 65, 59, 54, 49, 44, 40, 36, 32, 28, 25, 22, 19,
						   16, 14, 12, 10, 8, 7, 6, 5, 4, 4};
	const int sin_len = sizeof(sin_pos) / sizeof(sin_pos[0]);

	char *tune = (char *) "aurora.mod";

#ifndef __AMIGA__
	if (xmp_load_module_from_memory(ctx, (void *) moddata, moddatalen) < 0) {
		fprintf(stderr, "%s: error loading %s\n", argv[0], tune);
		exit(0);
	}

	xmp_start_player(ctx, SAMPLERATE, 0);
#else
	SND_LoadModuleFromMemory(moddata, moddatalen);
#endif

	SDL_PauseAudio(0);
	int fade = 255, i = 0;
	SDL_Rect screenWH = {0, 0, WIDTH, HEIGHT};

	while (gameRunning) {
		int j = 0;
		//Start the frame timer
		fps.start();
#ifndef __AMIGA
		if (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				gameRunning = false;
			}

			if (event.type == SDL_KEYDOWN) {
				SDLKey keyPressed = event.key.keysym.sym;
				switch (keyPressed) {
					case SDLK_ESCAPE:
						gameRunning = false;
						break;
					case SDLK_RETURN:
						if (scnCnt == 0) {
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
#else
		if (getKey() == 0x4000)
			gameRunning = false;

#endif
		SDL_FillRect(screen, NULL, 7);
		//exit(0);
		if (++sin_cnt >= sin_len) sin_cnt = 0;

		DrawStars(stars, screen);
		if ((frame / 60) > 18 && (frame / 60) < 28) {
//			scnCnt = 3;
			if (fade < 1) {
				fade = 0;

			} else {
				--fade;
			}

			SDL_Rect box = {0, 0, WIDTH, 5};
			for (int v = 0; v < HEIGHT / 5; v++) {
				box.y = v * 5;
				SDL_FillRect(screen, &box, (int)ceil(rand() / 254)+1);
			}
		} else {
			SDL_Rect box = {0, SCROLLER_Y_TOP / 2, WIDTH, 1};
			uint32_t color = 1;//SDL_MapRGB(screen->format, 0x7e, 0xd7, 0xe7);
			uint32_t bgcolor = 7;
			SDL_FillRect(screen, &box, color);
			box.y = SCROLLER_Y_TOP + (SCROLLER_Y_TOP / 2);
			SDL_FillRect(screen, &box, color);
			box.y = (SCROLLER_Y_TOP / 2) + 1;
			box.h = (SCROLLER_Y_TOP) - 1;
			SDL_FillRect(screen, &box, bgcolor);
		}

		if (scnCnt == 0) {
			scr = -((WIDTH / 2) - 8 * 8);
			for (const char &c : SCROLLER_TEXT[textCnt]) {
				int fnt = c - 65;
				int offsetY = 0;
				int letterX = 16*fnt - (((16 * fnt)/320)*320);
				int letterY = (int((16 * fnt)/320)*16);

				if ((16 * j) - scr < WIDTH && (16 * j) - scr > -16) {
					blit(font, screen, letterX, letterY, (int) ((16 * j) - scr), SCROLLER_Y_TOP + offsetY - 8, 16, 16);
				}

				j++;
			}

			DrawText(font, screen, (WIDTH / 2) - ((16 * 11) / 2), SCROLLER_Y_TOP + 32, 16, 16, "PRESS ENTER");

		} else if (scnCnt == 1) {
			for (const char &c : SCROLLER_TEXT[textCnt]) {
				int fnt = c - 65;
				int offsetY = 0;
				int letterX = 16*fnt - (((16 * fnt)/320)*320);
				int letterY = (int((16 * fnt)/320)*16);

				offsetY = sin_cnt + j;

				if (offsetY >= sin_len)
					offsetY -= sin_len;

				offsetY = (sin_pos[offsetY] - (149 / 2))/2;

				if ((16 * j) - scr < WIDTH && (16 * j) - scr > -16) {
					blit( font, screen, letterX, letterY, (int) ((16 * j) - scr), SCROLLER_Y_TOP + offsetY - 8, 16, 16);
				}

				j++;
			}

			scr = scr + SCROLLER_SPEED;
			if (scr > SCROLLER_TEXT[textCnt].size() * 32) {
				scr = -WIDTH;
				if (++textCnt >= SCROLLER_TEXT_LEN) {
					textCnt = 0;
					++scnCnt;
				}
			}
		} else if (scnCnt == 2) {
			DrawBalls(ball, screen);
		} else if (scnCnt == 3) {
			DrawText(font, screen, (WIDTH / 2) - ((16 * 7) / 2), SCROLLER_Y_TOP - 8, 16, 16, "LOADING");
		}

		SDL_Flip(screen);
		frame++;

		if (fps.get_ticks() < 1000 / FRAMES_PER_SECOND) {
			if (frame %2 && scnCnt == 2) {
				i++;
				SDL_SetColors(screen, colors[i%2], 0, 256);
			}
			SDL_Delay((1000 / FRAMES_PER_SECOND) - fps.get_ticks());
		}
	}

#ifndef __AMIGA__
	xmp_end_player(ctx);
	xmp_release_module(ctx);
	xmp_free_context(ctx);
	SDL_CloseAudio();
#else
	SND_EjectModule();
#endif
//	SDL_FreeSurface(ball);
//	SDL_FreeSurface(stars);
//	SDL_FreeSurface(font);
//	SDL_FreeSurface(font2);

	SDL_Quit();

	return 0;
}
