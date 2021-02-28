#ifndef TAKETHIS_INCLUDED_PLAYER
#define TAKETHIS_INCLUDED_PLAYER


#include "room.h"


typedef struct tt_player {
    tt_room *room;
    int xwalk;
    int ywalk;
    int x;
    int y;
    int xrem;
    int yrem;
    int variant;
    int rem;
    int money;
    int keys;
    int the_key;
} tt_player;


void tt_player_walk(int delta);

void tt_player_draw();


#endif
