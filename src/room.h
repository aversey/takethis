#ifndef TAKETHIS_INCLUDED_ROOM
#define TAKETHIS_INCLUDED_ROOM


#include <SDL2/SDL.h>


typedef struct tt_room {
    const SDL_Rect *tiles[15][19];
    void *bodies[15][19];
} tt_room;


tt_room *tt_room_load(int num);

void tt_room_draw(SDL_Renderer *rdr, SDL_Texture *txr, tt_room *room);

/*
void tt_room_rtol(SDL_Renderer *rdr, SDL_Texture *txr,
                  tt_room *right, tt_room *left, int permille);

void tt_room_utod(SDL_Renderer *rdr, SDL_Texture *txr,
                  tt_room *up, tt_room *down, int permille);
*/


#endif
