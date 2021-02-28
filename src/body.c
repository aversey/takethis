#include "body.h"

#include <SDL2/SDL_ttf.h>
#include "room.h"
#include "globals.h"


static void move(tt_body *b, int d)
{
    b->xrem += b->xvel * d;
    b->yrem += b->yvel * d;
    b->x += b->xrem / 1000;
    b->y += b->yrem / 1000;
    b->xrem = b->xrem % 1000;
    b->yrem = b->yrem % 1000;
}


void tt_body_move(int d)
{
    tt_room *r = ttplayer.room;
    int i, j;
    for (i = 0; i != TT_ROOM_H; ++i) {
        for (j = 0; j != TT_ROOM_W; ++j) {
            tt_body *w = r->walls[i][j];
            if (w) w->rem += d;
        }
    }
    for (i = 0; i != r->bodies_count; ++i) {
        tt_body *b = r->bodies + i;
        b->rem += d;
        int ex = b->x;
        int exr = b->xrem;
        int ey = b->y;
        int eyr = b->yrem;
        move(b, d);
        {
            SDL_Rect box = { b->x + 5, b->y + 2, 22, 28 };
            if (!tt_room_collide(r, &box) && !tt_room_out(r, &box)) continue;
        }
        b->x = ex;
        b->y = ey;
        b->xrem = exr;
        b->yrem = eyr;
        b->xvel *= -1;
        move(b, d);
        {
            SDL_Rect box = { b->x + 5, b->y + 2, 22, 28 };
            if (!tt_room_collide(r, &box) && !tt_room_out(r, &box)) continue;
        }
        b->x = ex;
        b->y = ey;
        b->xrem = exr;
        b->yrem = eyr;
        b->xvel *= -1;
        b->yvel *= -1;
        move(b, d);
        {
            SDL_Rect box = { b->x + 5, b->y + 2, 22, 28 };
            if (!tt_room_collide(r, &box) && !tt_room_out(r, &box)) continue;
        }
        b->x = ex;
        b->y = ey;
        b->xrem = exr;
        b->yrem = eyr;
        b->xvel *= -1;
        move(b, d);
    }
}
