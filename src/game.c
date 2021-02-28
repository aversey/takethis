#include "game.h"

#include <SDL2/SDL.h>
#include "globals.h"


int magic = 0;

static int ticks;

static void step(int d)
{
    int xw = ttplayer.xwalk * 137;
    int yw = ttplayer.ywalk * 137;
    if (xw || yw) {
        ttplayer.rem += d;
    }
    if (xw && yw) {
        ttplayer.xrem += (double)(xw * d) / sqrt(2);
        ttplayer.yrem += (double)(yw * d) / sqrt(2);
    } else if (xw || yw) {
        ttplayer.xrem += xw * d;
        ttplayer.yrem += yw * d;
    }
    int x = ttplayer.x;
    int y = ttplayer.y;
    ttplayer.x += ttplayer.xrem / 1000;
    ttplayer.y += ttplayer.yrem / 1000;
    ttplayer.xrem = ttplayer.xrem % 1000;
    ttplayer.yrem = ttplayer.yrem % 1000;
    {
        SDL_Rect box = { ttplayer.x, ttplayer.y, 32, 32 };
        int out = tt_room_out(ttplayer.room, &box);
        if (out) ttplayer.room = ttplayer.room->neighbours[out - 1];
        if (out == 1)      ttplayer.y = TT_ROOM_H * 32 - 32;
        else if (out == 2) ttplayer.x = 0;
        else if (out == 3) ttplayer.y = 0;
        else if (out == 4) ttplayer.x = TT_ROOM_W * 32 - 32;
    }
    {
        SDL_Rect box = { ttplayer.x, ttplayer.y, 32, 32 };
        if (tt_room_collide(ttplayer.room, &box)) {
            ttplayer.x = x;
            ttplayer.y = y;
        }
    }
    tt_body_move(d);
    {
        int i;
        SDL_Rect box = { ttplayer.x, ttplayer.y, 32, 32 };
        for (i = 0; i != ttplayer.room->bodies_count; ++i) {
            tt_body *b = ttplayer.room->bodies + i;
            SDL_Rect body = { 4 + b->x, 4 + b->y, 24, 24 };
            if (SDL_HasIntersection(&body, &box)) b->collision_act(b);
        }
    }
}


void gotofirstroom()
{
    ttplayer.room = ttmap + '0';
    magic = 0;
    ticks = SDL_GetTicks();
}

void tt_mainloop()
{
    int keyw, keys, keya, keyd, arru, arrr, arrd, arrl;
    keyw = keya = keys = keyd = arru = arrr = arrd = arrl = 0;
    ticks = SDL_GetTicks();
    int q = 0;
    while (!q) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) q = 1;
            else if (e.type == SDL_KEYDOWN) {
                int code = e.key.keysym.scancode;
                if (code == SDL_SCANCODE_W)          keyw = 1;
                else if (code == SDL_SCANCODE_S)     keys = 1;
                else if (code == SDL_SCANCODE_A)     keya = 1;
                else if (code == SDL_SCANCODE_D)     keyd = 1;
                else if (code == SDL_SCANCODE_UP)    arru = 1;
                else if (code == SDL_SCANCODE_RIGHT) arrr = 1;
                else if (code == SDL_SCANCODE_DOWN)  arrd = 1;
                else if (code == SDL_SCANCODE_LEFT)  arrl = 1;
            } else if (e.type == SDL_KEYUP) {
                int code = e.key.keysym.scancode;
                if (code == SDL_SCANCODE_W)          keyw = 0;
                else if (code == SDL_SCANCODE_S)     keys = 0;
                else if (code == SDL_SCANCODE_A)     keya = 0;
                else if (code == SDL_SCANCODE_D)     keyd = 0;
                else if (code == SDL_SCANCODE_UP)    arru = 0;
                else if (code == SDL_SCANCODE_RIGHT) arrr = 0;
                else if (code == SDL_SCANCODE_DOWN)  arrd = 0;
                else if (code == SDL_SCANCODE_LEFT)  arrl = 0;
            }
        }
        ttplayer.ywalk = (keys | arrd) - (keyw | arru);
        ttplayer.xwalk = (keyd | arrr) - (keya | arrl);
        int newticks = SDL_GetTicks();
        step(newticks - ticks);
        ticks = newticks;
        SDL_RenderClear(ttrdr);
        tt_player_draw();
        SDL_RenderPresent(ttrdr);
        if (magic == tt_gotofirstroom) {
            gotofirstroom();
        }
    }
}
