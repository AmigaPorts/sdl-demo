#ifndef _hex2surface_h
#define _hex2surface_h
#ifndef __AMIGA__
#include <SDL.h>
#else
#include "video.h"
#endif

#include "palette.h"

SDL_Surface* Hex2Surface(const unsigned char image[], int width, int height);
SDL_Surface* CreateSurface(int width, int height);
#endif