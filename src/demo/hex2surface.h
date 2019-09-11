#ifndef _hex2surface_h
#define _hex2surface_h

#include <SDL.h>

#include "palette.h"

SDL_Surface *Hex2Surface(const unsigned char* image, int width, int height);

SDL_Surface *CreateSurface(int width, int height);

#endif