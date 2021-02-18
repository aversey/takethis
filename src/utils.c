#include "utils.h"


SDL_Texture *load_texture(SDL_Renderer *renderer, const char *path)
{
    SDL_Surface *s = SDL_LoadBMP(path);
    SDL_Texture *res = SDL_CreateTextureFromSurface(renderer, s);
    SDL_FreeSurface(s);
    return res;
}
