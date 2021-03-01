#include "game.h"

#include <SDL2/SDL.h>
#include "globals.h"


int magic = 0;

static int ticks;

static const char *gulagmsg = "          GULAG HAS YOU";

static void directly_gulag(tt_body *b)
{
    gulagmsg = " Communism is Indestructable";
    magic = tt_gotogulag;
    Mix_PlayMusic(ussr, -1);
}

static void step(int d)
{
    int xw = ttplayer.xwalk * 200;
    int yw = ttplayer.ywalk * 200;
    if (xw || yw) {
        ttplayer.rem += d;
    }
    if (ttplayer.zhiv_lenin) {
        int oldpos = ttplayer.lenin_pos;
        ttplayer.lenin_rem += d;
        ttplayer.lenin_pos_rem += d * ttplayer.lenin_vel;
        ttplayer.lenin_pos += ttplayer.lenin_pos_rem / 1000;
        ttplayer.lenin_pos_rem = ttplayer.lenin_pos_rem % 1000;
        if (ttplayer.lenin_pos < 64 || 32 * 16 < ttplayer.lenin_pos) {
            ttplayer.lenin_pos = oldpos;
            ttplayer.lenin_vel *= -1;
        }
        static int lenin_grib = 0;
        if (ttplayer.lenin_rem % 1000 > 500) {
            if (!lenin_grib) {
                lenin_grib = 1;
                ttplayer.room->bodies_count++;
                ttplayer.room->bodies = realloc(
                    ttplayer.room->bodies,
                    sizeof(tt_body) * ttplayer.room->bodies_count
                );
                tt_body *b = ttplayer.room->bodies +
                             ttplayer.room->bodies_count - 1;
                b->x = ttplayer.lenin_pos;
                b->y = 32;
                b->xrem = 0;
                b->yrem = 0;
                b->rem = 0;
                b->txrrow = 7;
                b->txrcol = rand() % 4;
                b->anim = 4;
                b->rate = 150 + (rand() % 50 - 25);
                b->collision_act = directly_gulag;
                b->msg = 0;
                b->msglen = 0;
                b->yvel = rand() % 100 - 50;
                b->yvel = b->yvel < 0 ? b->yvel - 50 : b->yvel + 50;
                b->xvel = rand() % 100 - 50;
                b->xvel = b->yvel < 0 ? b->yvel - 50 : b->yvel + 50;
            }
        } else if (lenin_grib) lenin_grib = 0;
    }
    static int first_gulag = 1;
    if (ttplayer.tobein_gulag && ttplayer.until_gulag > 0) {
        if (first_gulag) {
            first_gulag = 0;
            Mix_PlayMusic(stalin, -1);
        }
        ttplayer.until_gulag -= d;
        if (ttplayer.until_gulag <= 0) {
            magic = tt_gotogulag;
            Mix_PlayMusic(ussr, -1);
        }
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
        if (out == 1)      ttplayer.y = TT_ROOM_H * 32 - 32;
        else if (out == 2) ttplayer.x = 0;
        else if (out == 3) ttplayer.y = 0;
        else if (out == 4) ttplayer.x = TT_ROOM_W * 32 - 32;
        if (out) {
            ttplayer.room = ttplayer.room->neighbours[out - 1];
            if (ttplayer.room == ttmap + 'L') {
                ttplayer.y -= 32;
                Mix_PlayMusic(lenin, -1);
                magic = tt_mausoleum;
            }
        }
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


static int q = 0;
static int keyw, keys, keya, keyd, arru, arrr, arrd, arrl;

static int min(int a, int b)  { return a < b ? a : b; }

static void gotofirstroom()
{
    SDL_BlendMode mode;
    SDL_GetRenderDrawBlendMode(ttrdr, &mode);
    int newticks = SDL_GetTicks();
    SDL_SetRenderDrawBlendMode(ttrdr, SDL_BLENDMODE_BLEND);
    while (!q && newticks < ticks + 800) {
        int delta = newticks - ticks;
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
        SDL_RenderClear(ttrdr);
        SDL_SetRenderDrawColor(ttrdr, 0, 0, 0, min(255, delta * 256 / 600));
        tt_player_draw();
        SDL_Rect d = { 14, 14, 32 * 20, 32 * 16 };
        SDL_RenderFillRect(ttrdr, &d);
        {
            SDL_Rect src = { 0, 16 * 5, 16, 16 };
            SDL_Rect dst = { 14 + ttplayer.x - delta +
                             rand() % delta,
                             14 + ttplayer.y - delta +
                             rand() % delta,
                             32 + delta / 2 + (rand() % delta),
                             32 + delta / 2 + (rand() % delta) };
            SDL_RenderCopy(ttrdr, tttxr, &src, &dst);
        }
        SDL_RenderPresent(ttrdr);
        newticks = SDL_GetTicks();
        SDL_SetRenderDrawColor(ttrdr, 0, 0, 0, 255);
    }
    SDL_SetRenderDrawBlendMode(ttrdr, mode);
    ttplayer.room = ttmap + '0';
    magic = 0;
    ticks = SDL_GetTicks();
}

static void gotogulag()
{
    SDL_BlendMode mode;
    SDL_GetRenderDrawBlendMode(ttrdr, &mode);
    int newticks = SDL_GetTicks();
    SDL_SetRenderDrawBlendMode(ttrdr, SDL_BLENDMODE_BLEND);
    while (!q && newticks < ticks + 5000) {
        int delta = newticks - ticks;
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
        static int roomchanged = 0;
        SDL_RenderClear(ttrdr);
        if (delta < 4200) SDL_SetRenderDrawColor(ttrdr, 0, 0, 0,
                                                 min(255, delta * 256 / 800));
        else {
            if (!roomchanged) {
                roomchanged = 1;
                ttplayer.room = ttmap + 'G';
                ttplayer.zhiv_lenin = 0;
            }
            SDL_SetRenderDrawColor(ttrdr, 0, 0, 0,
                                   255 - (delta - 4200) * 256 / 800);
        }
        tt_player_draw();
        SDL_Rect d = { 14, 14, 32 * 20, 32 * 16 };
        SDL_RenderFillRect(ttrdr, &d);
        if (delta < 4200) {
            SDL_Color c = { 128, 20, 20, min(255, delta * 256 / 800) };
            SDL_Surface *surf = TTF_RenderText_Blended(
                    ttfont, gulagmsg, c);
            SDL_Texture *t = SDL_CreateTextureFromSurface(ttrdr, surf);
            SDL_Rect dst = { 14, 14 + 32 * 7,
                             surf->w * 2, surf->h * 2 };
            SDL_RenderCopy(ttrdr, t, 0, &dst);
            SDL_DestroyTexture(t);
            SDL_FreeSurface(surf);
        } else {
            SDL_Color c = { 128, 20, 20, 255 - (delta - 4200) * 256 / 800 };
            SDL_Surface *surf = TTF_RenderText_Blended(
                    ttfont, gulagmsg, c);
            SDL_Texture *t = SDL_CreateTextureFromSurface(ttrdr, surf);
            SDL_Rect dst = { 14, 14 + 32 * 7,
                             surf->w * 2, surf->h * 2 };
            SDL_RenderCopy(ttrdr, t, 0, &dst);
            SDL_DestroyTexture(t);
            SDL_FreeSurface(surf);
        }
        SDL_RenderPresent(ttrdr);
        newticks = SDL_GetTicks();
        SDL_SetRenderDrawColor(ttrdr, 0, 0, 0, 255);
    }
    SDL_SetRenderDrawBlendMode(ttrdr, mode);
    magic = 0;
    ticks = SDL_GetTicks();
}

static void mausoleum()
{
    int newticks = SDL_GetTicks();
    while (!q && newticks < ticks + 14300) {
        int delta = newticks - ticks;
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
        SDL_RenderClear(ttrdr);
        tt_player_draw();
        {
            int lenin_size = delta * (48 - 4 - 6) / 14300;
            SDL_Rect src = { 32, 16 * 11 + 4, 32, lenin_size };
            SDL_Rect dst = { 14 + ttplayer.lenin_pos,
                             14 + 32 + 96 - 8 - lenin_size * 2,
                             64, lenin_size * 2 };
            SDL_RenderCopy(ttrdr, tttxr, &src, &dst);
        }
        {
            SDL_Rect src = { 96, 16 * 11, 64, 48 };
            SDL_Rect dst = { 14 + ttplayer.lenin_pos - 32,
                             110, 128, 96 };
            SDL_RenderCopy(ttrdr, tttxr, &src, &dst);
        }
        SDL_RenderPresent(ttrdr);
        newticks = SDL_GetTicks();
    }
    ttplayer.zhiv_lenin = 1;
    magic = 0;
    ticks = SDL_GetTicks();
}

void tt_mainloop()
{
    keyw = keya = keys = keyd = arru = arrr = arrd = arrl = 0;
    ticks = SDL_GetTicks();
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
        } else if (magic == tt_gotogulag) {
            gotogulag();
        } else if (magic == tt_mausoleum) {
            mausoleum();
        }
    }
}
