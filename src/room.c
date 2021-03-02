#include "room.h"

int tt_room_collide(tt_room *room, SDL_Rect *box)
{
    SDL_Rect r = { 0, 0, 32, 16 };
    int      i, j;
    for (i = 0; i != TT_ROOM_H; ++i) {
        r.y = 32 * i;
        for (j = 0; j != TT_ROOM_W; ++j) {
            r.x = 32 * j;
            if (SDL_HasIntersection(box, room->walls[i][j] ? &r : 0))
                return 1;
        }
    }
    return 0;
}

int tt_room_out(tt_room *room, SDL_Rect *box)
{
    if (!box) return 0;
    if (box->y < 0) return 1;
    if (box->x + box->w > TT_ROOM_W * 32) return 2;
    if (box->y + box->h > TT_ROOM_H * 32) return 3;
    if (box->x < 0) return 4;
    return 0;
}
