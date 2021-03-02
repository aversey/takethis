#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdlib.h>
#include <time.h>
#include "globals.h"
#include "map.h"
#include "player.h"
#include "game.h"


static SDL_Texture *loadtxr(const char *path)
{
    SDL_Surface *surf = SDL_LoadBMP(path);
    SDL_Texture *res = SDL_CreateTextureFromSurface(ttrdr, surf);
    SDL_FreeSurface(surf);
    return res;
}

int main(int argc, char **argv)
{
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    Mix_Init(MIX_INIT_OGG);
    Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 4096);
    SDL_Window *wdw = SDL_CreateWindow("T A K E T H I S",
                                       SDL_WINDOWPOS_UNDEFINED,
                                       SDL_WINDOWPOS_UNDEFINED,
                                       950, 540, 0);
    ttrdr = SDL_CreateRenderer(wdw, -1, SDL_RENDERER_ACCELERATED);
    tttxr = loadtxr("data/txr.bmp");
    ttfont = TTF_OpenFont("data/font.otf", 24);
    grib = Mix_LoadMUS("data/grib.ogg");
    ussr = Mix_LoadMUS("data/ussr.ogg");
    stalin = Mix_LoadMUS("data/stalin.ogg");
    lenin = Mix_LoadMUS("data/lenin.ogg");
    curmus = 0;
    tt_map_load();

    srand(time(0));
    SDL_SetRenderDrawColor(ttrdr, 0, 0, 0, 255);

    tt_mainloop();

    tt_map_free();
    TTF_CloseFont(ttfont);
    SDL_DestroyTexture(tttxr);
    SDL_DestroyRenderer(ttrdr);
    SDL_DestroyWindow(wdw);
    Mix_HaltMusic();
    Mix_FreeMusic(lenin);
    Mix_FreeMusic(stalin);
    Mix_FreeMusic(ussr);
    Mix_FreeMusic(grib);
    Mix_CloseAudio();
    TTF_Quit();
    SDL_Quit();
    return 0;
}
