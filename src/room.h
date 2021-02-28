#ifndef TAKETHIS_INCLUDED_ROOM
#define TAKETHIS_INCLUDED_ROOM


#include <SDL2/SDL.h>
#include "body.h"


#define TT_ROOM_W 20
#define TT_ROOM_H 16

typedef struct tt_room {
    SDL_Rect *floor[TT_ROOM_H][TT_ROOM_W];
    tt_body *walls[TT_ROOM_H][TT_ROOM_W];
    SDL_Rect *roof[TT_ROOM_H][TT_ROOM_W];
    tt_body *bodies;
    int bodies_count;
    struct tt_room *neighbours[4];
} tt_room;


void tt_room_draw_background(tt_room *room);
void tt_room_draw_foreground(tt_room *room);

int tt_room_collide(tt_room *room, SDL_Rect *box);

int tt_room_out(tt_room *room, SDL_Rect *box);


#endif
