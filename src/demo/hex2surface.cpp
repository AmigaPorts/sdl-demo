#include "hex2surface.h"
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
Uint32 rmask = 0xff000000;
Uint32 gmask = 0x00ff0000;
Uint32 bmask = 0x0000ff00;
Uint32 amask = 0x000000ff;
#else
Uint32 rmask = 0x000000ff;
Uint32 gmask = 0x0000ff00;
Uint32 bmask = 0x00ff0000;
Uint32 amask = 0xff000000;
#endif

SDL_Surface* Hex2Surface(const unsigned char image[], int width, int height)
{
	auto *surface = SDL_CreateRGBSurfaceFrom((void*)image, width, height, 8, width * 1, rmask, gmask, bmask, amask);
	SDL_SetPalette(surface,SDL_LOGPAL|SDL_PHYSPAL, colors, 0, 256);
	return surface;
}

SDL_Surface* CreateSurface(int width, int height)
{
	return SDL_CreateRGBSurface(0, width, height, 8, rmask, gmask, bmask, amask);
}