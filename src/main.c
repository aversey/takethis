#include "game.h"
#include "globals.h"
#include "map.h"
#include "player.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include <time.h>

static SDL_Texture *loadtxr(const char *path)
{
    SDL_Surface *surf = SDL_LoadBMP(path);
    SDL_Texture *res  = SDL_CreateTextureFromSurface(ttrdr, surf);
    SDL_FreeSurface(surf);
    return res;
}

int main(int argc, char **argv)
{
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    Mix_Init(MIX_INIT_OGG);
    Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 4096);
    Mix_AllocateChannels(4);
    ttwdw = SDL_CreateWindow("T A K E T H I S", SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED, 950, 540, 0);
    ttrdr = SDL_CreateRenderer(
        ttwdw, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
    tttxr    = loadtxr("data/txr.bmp");
    lighttxr = SDL_CreateTexture(ttrdr, SDL_PIXELFORMAT_RGBA8888,
                                 SDL_TEXTUREACCESS_TARGET, 950, 540);
    SDL_SetTextureBlendMode(lighttxr, SDL_BLENDMODE_ADD);
    ttfont     = TTF_OpenFont("data/font.otf", 24);
    grib       = Mix_LoadMUS("data/grib.ogg");
    ussr       = Mix_LoadMUS("data/ussr.ogg");
    stalin     = Mix_LoadMUS("data/stalin.ogg");
    lenin      = Mix_LoadMUS("data/lenin.ogg");
    ttcoin     = Mix_LoadWAV("data/coin.wav");
    ttdoor     = Mix_LoadWAV("data/door.wav");
    ttlenin    = Mix_LoadWAV("data/lenin.wav");
    tthadouken = Mix_LoadWAV("data/hadouken.wav");
    curmus     = 0;
    tt_map_load();

    srand(time(0));
    SDL_SetRenderDrawColor(ttrdr, 0, 0, 0, 255);

    tt_mainloop();

    tt_map_free();
    TTF_CloseFont(ttfont);
    SDL_DestroyTexture(tttxr);
    SDL_DestroyRenderer(ttrdr);
    SDL_DestroyWindow(ttwdw);
    Mix_HaltMusic();
    Mix_FreeChunk(tthadouken);
    Mix_FreeChunk(ttlenin);
    Mix_FreeChunk(ttcoin);
    Mix_FreeChunk(ttdoor);
    Mix_FreeMusic(lenin);
    Mix_FreeMusic(stalin);
    Mix_FreeMusic(ussr);
    Mix_FreeMusic(grib);
    Mix_CloseAudio();
    TTF_Quit();
    SDL_Quit();
    return 0;
}
