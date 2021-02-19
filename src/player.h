#ifndef TAKETHIS_INCLUDED_ROOM
#define TAKETHIS_INCLUDED_ROOM


typedef struct tt_room {
    SDL_Rect *tiles[15][19];
    tt_body *bodies[15][19];
} tt_room;


tt_room *tt_room_load(int num);

void tt_room_draw(tt_room *room);

void tt_room_rtol(tt_room *right, tt_room *left, int permille);

void tt_room_utod(tt_room *up, tt_room *down, int permille);


#endif
