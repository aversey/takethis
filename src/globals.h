#ifndef TT_INCLUDED_GLOBALS
#define TT_INCLUDED_GLOBALS

#include "player.h"
#include "room.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

extern SDL_Window *ttwdw;

extern TTF_Font *ttfont;

extern SDL_Texture *tttxr;

extern SDL_Renderer *ttrdr;

extern Mix_Chunk *ttlenin;
extern Mix_Chunk *ttcoin;
extern Mix_Chunk *ttdoor;
extern Mix_Music *grib;
extern Mix_Music *ussr;
extern Mix_Music *stalin;
extern Mix_Music *lenin;
extern Mix_Music *curmus;

extern tt_room ttmap[256];

extern tt_player ttplayer;

#endif
