#ifndef TT_INCLUDED_GLOBALS
#define TT_INCLUDED_GLOBALS


#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include "room.h"
#include "player.h"


extern TTF_Font *ttfont;

extern SDL_Texture *tttxr;

extern SDL_Renderer *ttrdr;

extern Mix_Music *ponpon;

extern tt_room ttmap[256];

extern tt_player ttplayer;


#endif
