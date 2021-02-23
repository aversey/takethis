#ifndef TAKETHIS_INCLUDED_ROOM
#define TAKETHIS_INCLUDED_ROOM


#include <SDL2/SDL.h>


#define TT_ROOM_W 19
#define TT_ROOM_H 15

typedef struct tt_room {
    char tiletypes[TT_ROOM_H][TT_ROOM_W];
    const SDL_Rect *tiles[TT_ROOM_H][TT_ROOM_W];
    void *bodies[TT_ROOM_H][TT_ROOM_W];
} tt_room;


tt_room *tt_room_load(char c);

void tt_room_draw(SDL_Renderer *rdr, SDL_Texture *txr, tt_room *room);

/*
void tt_room_rtol(SDL_Renderer *rdr, SDL_Texture *txr,
                  tt_room *right, tt_room *left, int permille);

void tt_room_utod(SDL_Renderer *rdr, SDL_Texture *txr,
                  tt_room *up, tt_room *down, int permille);
*/


#endif
