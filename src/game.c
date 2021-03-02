#include "game.h"

#include "globals.h"
#include "map.h"
#include <SDL2/SDL.h>
#include <stdio.h>

int        magic                = 0;
static int first_gulag          = 1;
static int fullscreen           = 0;
static int lenin_grib           = 0;
static int lenin_until_hadouken = 3;

static int ticks;

static char *readline(FILE *f)
{
    int   c      = fgetc(f);
    char *res    = 0;
    int   reslen = 0;
    while (c != EOF && c != '\n') {
        ++reslen;
        res             = realloc(res, reslen + 1);
        res[reslen - 1] = c;
        c               = fgetc(f);
    }
    res[reslen] = 0;
    return res;
}

static void outnum(FILE *f, int n)
{
    if (n) {
        if (n < 0) {
            fputc('-', f);
            n *= -1;
        }
        int pos = 1;
        while (n / pos) pos *= 10;
        if (pos > 1) pos /= 10;
        while (pos) {
            fputc('0' + n / pos, f);
            n = n % pos;
            pos /= 10;
        }
    }
    fputc('\n', f);
}

static int readnum(FILE *f)
{
    int sign = 1;
    int n    = 0;
    int c    = fgetc(f);
    if (c == '-') {
        sign = -1;
        c    = fgetc(f);
    }
    while (c != EOF && c != '\n') {
        n *= 10;
        n += c - '0';
        c = fgetc(f);
    }
    return sign * n;
}

static void save()
{
    FILE *f = fopen("save", "w");
    outnum(f, first_gulag);
    outnum(f, lenin_grib);
    outnum(f, lenin_until_hadouken);
    if (curmus == grib) fputs("g\n", f);
    else if (curmus == ussr)
        fputs("u\n", f);
    else if (curmus == stalin)
        fputs("s\n", f);
    else if (curmus == lenin)
        fputs("l\n", f);
    else
        fputs("0\n", f);
    fputc(ttplayer.room - ttmap, f);
    fputc('\n', f);
    outnum(f, ttplayer.x);
    outnum(f, ttplayer.y);
    outnum(f, ttplayer.xrem);
    outnum(f, ttplayer.yrem);
    outnum(f, ttplayer.variant);
    outnum(f, ttplayer.rem);
    outnum(f, ttplayer.money);
    outnum(f, ttplayer.keys[0]);
    outnum(f, ttplayer.keys[1]);
    outnum(f, ttplayer.tobein_gulag);
    outnum(f, ttplayer.until_gulag);
    outnum(f, ttplayer.lenin_pos);
    outnum(f, ttplayer.lenin_pos_rem);
    outnum(f, ttplayer.lenin_vel);
    outnum(f, ttplayer.lenin_zhiv);
    outnum(f, ttplayer.lenin_rem);
    int i;
    for (i = '0'; i != '~'; ++i) {
        int p, q;
        for (p = 0; p != TT_ROOM_H; ++p) {
            for (q = 0; q != TT_ROOM_W; ++q) {
                SDL_Rect *r = ttmap[i].floor[p][q];
                if (r) {
                    fputs("1\n", f);
                    outnum(f, r->x);
                    outnum(f, r->y);
                    outnum(f, r->w);
                    outnum(f, r->h);
                } else
                    fputc('\n', f);
                tt_body *w = ttmap[i].walls[p][q];
                if (w) {
                    fputs("1\n", f);
                    outnum(f, w->x);
                    outnum(f, w->y);
                    outnum(f, w->xrem);
                    outnum(f, w->yrem);
                    outnum(f, w->xvel);
                    outnum(f, w->yvel);
                    outnum(f, w->txrrow);
                    outnum(f, w->txrcol);
                    outnum(f, w->rem);
                    outnum(f, w->anim);
                    outnum(f, w->rate);
                    outnum(f, w->collision_act);
                    outnum(f, w->isdoor);
                    outnum(f, w->msglen);
                    if (w->msg) fputs(w->msg, f);
                    fputc('\n', f);
                } else
                    fputc('\n', f);
            }
        }
        outnum(f, ttmap[i].bodies_count);
        for (p = 0; p != ttmap[i].bodies_count; ++p) {
            tt_body *b = ttmap[i].bodies + p;
            outnum(f, b->x);
            outnum(f, b->y);
            outnum(f, b->xrem);
            outnum(f, b->yrem);
            outnum(f, b->xvel);
            outnum(f, b->yvel);
            outnum(f, b->txrrow);
            outnum(f, b->txrcol);
            outnum(f, b->rem);
            outnum(f, b->anim);
            outnum(f, b->rate);
            outnum(f, b->collision_act);
            outnum(f, b->isdoor);
            outnum(f, b->msglen);
            if (b->msg) fputs(b->msg, f);
            fputc('\n', f);
        }
        fputc(ttmap[i].neighbours[0] - ttmap, f);
        fputc(ttmap[i].neighbours[1] - ttmap, f);
        fputc(ttmap[i].neighbours[2] - ttmap, f);
        fputc(ttmap[i].neighbours[3] - ttmap, f);
        fputc('\n', f);
    }
}

static void load()
{
    FILE *f = fopen("save", "r");
    if (!f) return;
    first_gulag          = readnum(f);
    lenin_grib           = readnum(f);
    lenin_until_hadouken = readnum(f);
    int c                = fgetc(f);
    if (c == 'g') {
        if (curmus != grib) {
            curmus = grib;
            Mix_PlayMusic(grib, -1);
        }
    } else if (c == 'u') {
        if (curmus != ussr) {
            curmus = ussr;
            Mix_PlayMusic(ussr, -1);
        }
    } else if (c == 's') {
        if (curmus != stalin) {
            curmus = stalin;
            Mix_PlayMusic(stalin, -1);
        }
    } else if (c == 'l') {
        if (curmus != lenin) {
            curmus = lenin;
            Mix_PlayMusic(lenin, -1);
        }
    } else {
        curmus = 0;
        Mix_PauseMusic();
    }
    fgetc(f);
    ttplayer.room = ttmap + fgetc(f);
    fgetc(f);
    ttplayer.x             = readnum(f);
    ttplayer.y             = readnum(f);
    ttplayer.xrem          = readnum(f);
    ttplayer.yrem          = readnum(f);
    ttplayer.variant       = readnum(f);
    ttplayer.rem           = readnum(f);
    ttplayer.money         = readnum(f);
    ttplayer.keys[0]       = readnum(f);
    ttplayer.keys[1]       = readnum(f);
    ttplayer.tobein_gulag  = readnum(f);
    ttplayer.until_gulag   = readnum(f);
    ttplayer.lenin_pos     = readnum(f);
    ttplayer.lenin_pos_rem = readnum(f);
    ttplayer.lenin_vel     = readnum(f);
    ttplayer.lenin_zhiv    = readnum(f);
    ttplayer.lenin_rem     = readnum(f);
    tt_map_free();
    int i;
    for (i = '0'; i != '~'; ++i) {
        int p, q;
        for (p = 0; p != TT_ROOM_H; ++p) {
            for (q = 0; q != TT_ROOM_W; ++q) {
                if (readnum(f)) {
                    ttmap[i].floor[p][q]    = malloc(sizeof(SDL_Rect));
                    ttmap[i].floor[p][q]->x = readnum(f);
                    ttmap[i].floor[p][q]->y = readnum(f);
                    ttmap[i].floor[p][q]->w = readnum(f);
                    ttmap[i].floor[p][q]->h = readnum(f);
                } else
                    ttmap[i].floor[p][q] = 0;
                if (readnum(f)) {
                    tt_body *b = ttmap[i].walls[p][q] = malloc(sizeof(*b));
                    b->x                              = readnum(f);
                    b->y                              = readnum(f);
                    b->xrem                           = readnum(f);
                    b->yrem                           = readnum(f);
                    b->xvel                           = readnum(f);
                    b->yvel                           = readnum(f);
                    b->txrrow                         = readnum(f);
                    b->txrcol                         = readnum(f);
                    b->rem                            = readnum(f);
                    b->anim                           = readnum(f);
                    b->rate                           = readnum(f);
                    b->collision_act                  = readnum(f);
                    b->isdoor                         = readnum(f);
                    b->msglen                         = readnum(f);
                    if (b->msglen) b->msg = readline(f);
                    else {
                        b->msg = 0;
                        fgetc(f);
                    }
                } else
                    ttmap[i].walls[p][q] = 0;
            }
        }
        ttmap[i].bodies_count = readnum(f);
        ttmap[i].bodies = malloc(ttmap[i].bodies_count * sizeof(tt_body));
        for (p = 0; p != ttmap[i].bodies_count; ++p) {
            tt_body *b       = ttmap[i].bodies + p;
            b->x             = readnum(f);
            b->y             = readnum(f);
            b->xrem          = readnum(f);
            b->yrem          = readnum(f);
            b->xvel          = readnum(f);
            b->yvel          = readnum(f);
            b->txrrow        = readnum(f);
            b->txrcol        = readnum(f);
            b->rem           = readnum(f);
            b->anim          = readnum(f);
            b->rate          = readnum(f);
            b->collision_act = readnum(f);
            b->isdoor        = readnum(f);
            b->msglen        = readnum(f);
            if (b->msglen) b->msg = readline(f);
            else {
                b->msg = 0;
                fgetc(f);
            }
        }
        ttmap[i].neighbours[0] = ttmap + fgetc(f);
        ttmap[i].neighbours[1] = ttmap + fgetc(f);
        ttmap[i].neighbours[2] = ttmap + fgetc(f);
        ttmap[i].neighbours[3] = ttmap + fgetc(f);
        fgetc(f);
    }
}

static const char *gulagmsg = "          GULAG HAS YOU";

static void directly_gulag(tt_body *b)
{
    gulagmsg = " Communism is Indestructable";
    magic    = tt_gotogulag;
    Mix_PlayMusic(ussr, -1);
    curmus = ussr;
}

static void togulag(tt_body *b)
{
    ttplayer.tobein_gulag = 1;
    b->collision_act      = 0;
    b->anim               = 1;
    b->txrrow             = 0;
    b->txrcol             = 15;
    b->msg                = 0;
    b->msglen             = 0;
    Mix_PlayChannel(-1, ttcoin, 0);
    ttplayer.money++;
}

static void gribtake(tt_body *b)
{
    curmus = grib;
    Mix_PlayMusic(grib, -1);
    b->collision_act = 0;
    b->anim          = 1;
    b->txrrow        = 0;
    b->txrcol        = 15;
    b->msg           = 0;
    b->msglen        = 0;
    ttplayer.variant = 1;
    magic            = tt_gotofirstroom;
}

static void keytake(tt_body *b)
{
    ttplayer.keys[b->txrcol]++;
    b->collision_act = 0;
    b->txrrow        = 0;
    b->txrcol        = 15;
    b->msg           = 0;
    b->msglen        = 0;
    Mix_PlayChannel(-1, ttcoin, 0);
}

static void doorcol(tt_body *b)
{
    if (ttplayer.keys[b->txrrow - 4]) {
        int      i;
        tt_room *r    = ttplayer.room;
        tt_body *part = 0;
        for (i = 0; i != r->bodies_count; i++) {
            part   = r->bodies + i;
            int dx = (part->x - b->x) / 32;
            int xe = (part->x - b->x) % 32;
            int dy = (part->y - b->y) / 32;
            int ye = (part->y - b->y) % 32;
            if (part->isdoor && !xe && !ye &&
                ((dx == 1 || dx == -1) ^ (dy == 1 || dy == -1)))
                break;
        }
        free(r->walls[b->y / 32][b->x / 32]);
        r->walls[b->y / 32][b->x / 32] = 0;
        free(r->walls[part->y / 32][part->x / 32]);
        r->walls[part->y / 32][part->x / 32] = 0;
        ttplayer.keys[b->txrrow - 4]--;
        b->collision_act    = 0;
        b->txrrow           = 0;
        b->txrcol           = 15;
        b->msg              = 0;
        b->msglen           = 0;
        part->collision_act = 0;
        part->txrrow        = 0;
        part->txrcol        = 15;
        part->msg           = 0;
        part->msglen        = 0;
        Mix_PlayChannel(-1, ttdoor, 0);
    }
}

static void step(int d)
{
    int xw = ttplayer.xwalk * 200;
    int yw = ttplayer.ywalk * 200;
    if (xw || yw) { ttplayer.rem += d; }
    if (ttplayer.lenin_zhiv) {
        int oldpos = ttplayer.lenin_pos;
        ttplayer.lenin_rem += d;
        ttplayer.lenin_pos_rem += d * ttplayer.lenin_vel;
        ttplayer.lenin_pos += ttplayer.lenin_pos_rem / 1000;
        ttplayer.lenin_pos_rem = ttplayer.lenin_pos_rem % 1000;
        if (ttplayer.lenin_pos < 64 || 32 * 16 < ttplayer.lenin_pos) {
            ttplayer.lenin_pos = oldpos;
            ttplayer.lenin_vel *= -1;
        }
        if (ttplayer.lenin_rem % 1000 > 500) {
            if (!lenin_grib) {
                lenin_grib = 1;
                ttplayer.room->bodies_count++;
                ttplayer.room->bodies =
                    realloc(ttplayer.room->bodies,
                            sizeof(tt_body) * ttplayer.room->bodies_count);
                tt_body *b =
                    ttplayer.room->bodies + ttplayer.room->bodies_count - 1;
                b->x             = ttplayer.lenin_pos;
                b->y             = 32;
                b->xrem          = 0;
                b->yrem          = 0;
                b->rem           = 0;
                b->anim          = 4;
                b->collision_act = colact_instgulag;
                b->msg           = 0;
                b->msglen        = 0;
                if (!lenin_until_hadouken) {
                    b->txrrow        = 7;
                    b->txrcol        = 2 + rand() % 2;
                    b->rate = 75 + (rand() % 50 - 25);
                    b->yvel = rand() % 50 - 25;
                    b->yvel = b->yvel < 0 ? b->yvel - 125 : b->yvel + 125;
                    b->xvel = rand() % 50 - 25;
                    b->xvel = b->yvel < 0 ? b->yvel - 125 : b->yvel + 125;
                    Mix_PlayChannel(-1, tthadouken, 0);
                    lenin_until_hadouken = 3;
                } else {
                    b->txrrow        = 8;
                    b->txrcol        = rand() % 4;
                    b->rate = 150 + (rand() % 50 - 25);
                    b->yvel = rand() % 100 - 50;
                    b->yvel = b->yvel < 0 ? b->yvel - 50 : b->yvel + 50;
                    b->xvel = rand() % 100 - 50;
                    b->xvel = b->yvel < 0 ? b->yvel - 50 : b->yvel + 50;
                    Mix_PlayChannel(-1, ttlenin, 0);
                    lenin_until_hadouken--;
                }
            }
        } else if (lenin_grib)
            lenin_grib = 0;
    }
    if (ttplayer.tobein_gulag && ttplayer.until_gulag > 0) {
        if (first_gulag) {
            first_gulag = 0;
            Mix_PlayMusic(stalin, -1);
            curmus = stalin;
        }
        ttplayer.until_gulag -= d;
        if (ttplayer.until_gulag <= 0) {
            magic = tt_gotogulag;
            Mix_PlayMusic(ussr, -1);
            curmus = ussr;
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
        int      out = tt_room_out(ttplayer.room, &box);
        if (out) {
            ttplayer.money--;
            if (ttplayer.room->neighbours[out - 1] == ttmap + 'L') {
                Mix_PauseMusic();
                curmus = 0;
                magic  = tt_mausoleum;
            } else {
                magic = tt_changeroom + out - 1;
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
        int      i;
        SDL_Rect box = { ttplayer.x, ttplayer.y, 32, 32 };
        for (i = 0; i != ttplayer.room->bodies_count; ++i) {
            tt_body *b = ttplayer.room->bodies + i;
            if (b->isdoor) {
                SDL_Rect body = { b->x - 2, b->y - 2, 36, 36 };
                if (SDL_HasIntersection(&body, &box)) {
                    switch (b->collision_act) {
                    case colact_grib: gribtake(b); break;
                    case colact_gulag: togulag(b); break;
                    case colact_instgulag: directly_gulag(b); break;
                    case colact_key: keytake(b); break;
                    case colact_door: doorcol(b); break;
                    }
                }
            } else {
                SDL_Rect body = { 4 + b->x, 4 + b->y, 24, 24 };
                if (SDL_HasIntersection(&body, &box)) {
                    switch (b->collision_act) {
                    case colact_grib: gribtake(b); break;
                    case colact_gulag: togulag(b); break;
                    case colact_instgulag: directly_gulag(b); break;
                    case colact_key: keytake(b); break;
                    case colact_door: doorcol(b); break;
                    }
                }
            }
        }
    }
}

static int q = 0;
static int keyw, keys, keya, keyd, arru, arrr, arrd, arrl;

static int min(int a, int b) { return a < b ? a : b; }

static void gotofirstroom()
{
    SDL_BlendMode mode;
    SDL_GetRenderDrawBlendMode(ttrdr, &mode);
    int newticks = SDL_GetTicks();
    SDL_SetRenderDrawBlendMode(ttrdr, SDL_BLENDMODE_BLEND);
    while (!q && newticks < ticks + 800) {
        int       delta = newticks - ticks;
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) q = 1;
            else if (e.type == SDL_KEYDOWN) {
                int code = e.key.keysym.scancode;
                if (code == SDL_SCANCODE_W) keyw = 1;
                else if (code == SDL_SCANCODE_S)
                    keys = 1;
                else if (code == SDL_SCANCODE_A)
                    keya = 1;
                else if (code == SDL_SCANCODE_D)
                    keyd = 1;
                else if (code == SDL_SCANCODE_UP)
                    arru = 1;
                else if (code == SDL_SCANCODE_RIGHT)
                    arrr = 1;
                else if (code == SDL_SCANCODE_DOWN)
                    arrd = 1;
                else if (code == SDL_SCANCODE_LEFT)
                    arrl = 1;
                else if (code == SDL_SCANCODE_F11) {
                    SDL_SetWindowFullscreen(
                        ttwdw, fullscreen ? 0 : SDL_WINDOW_FULLSCREEN);
                    fullscreen = !fullscreen;
                }
            } else if (e.type == SDL_KEYUP) {
                int code = e.key.keysym.scancode;
                if (code == SDL_SCANCODE_W) keyw = 0;
                else if (code == SDL_SCANCODE_S)
                    keys = 0;
                else if (code == SDL_SCANCODE_A)
                    keya = 0;
                else if (code == SDL_SCANCODE_D)
                    keyd = 0;
                else if (code == SDL_SCANCODE_UP)
                    arru = 0;
                else if (code == SDL_SCANCODE_RIGHT)
                    arrr = 0;
                else if (code == SDL_SCANCODE_DOWN)
                    arrd = 0;
                else if (code == SDL_SCANCODE_LEFT)
                    arrl = 0;
            }
        }
        SDL_RenderClear(ttrdr);
        SDL_SetRenderDrawColor(ttrdr, 0, 0, 0, min(255, delta * 256 / 600));
        tt_player_draw();
        SDL_Rect d = { 14, 14, 32 * 20, 32 * 16 };
        SDL_RenderFillRect(ttrdr, &d);
        {
            SDL_Rect src = { 0, 16 * 6, 16, 16 };
            SDL_Rect dst = { 14 + ttplayer.x - delta + rand() % delta,
                             14 + ttplayer.y - delta + rand() % delta,
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
    magic         = 0;
    ticks         = SDL_GetTicks();
}

static void gotogulag()
{
    int           roomchanged = 0;
    SDL_BlendMode mode;
    SDL_GetRenderDrawBlendMode(ttrdr, &mode);
    int newticks = SDL_GetTicks();
    SDL_SetRenderDrawBlendMode(ttrdr, SDL_BLENDMODE_BLEND);
    while (!q && newticks < ticks + 5000) {
        int       delta = newticks - ticks;
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) q = 1;
            else if (e.type == SDL_KEYDOWN) {
                int code = e.key.keysym.scancode;
                if (code == SDL_SCANCODE_W) keyw = 1;
                else if (code == SDL_SCANCODE_S)
                    keys = 1;
                else if (code == SDL_SCANCODE_A)
                    keya = 1;
                else if (code == SDL_SCANCODE_D)
                    keyd = 1;
                else if (code == SDL_SCANCODE_UP)
                    arru = 1;
                else if (code == SDL_SCANCODE_RIGHT)
                    arrr = 1;
                else if (code == SDL_SCANCODE_DOWN)
                    arrd = 1;
                else if (code == SDL_SCANCODE_LEFT)
                    arrl = 1;
                else if (code == SDL_SCANCODE_F11) {
                    SDL_SetWindowFullscreen(
                        ttwdw, fullscreen ? 0 : SDL_WINDOW_FULLSCREEN);
                    fullscreen = !fullscreen;
                }
            } else if (e.type == SDL_KEYUP) {
                int code = e.key.keysym.scancode;
                if (code == SDL_SCANCODE_W) keyw = 0;
                else if (code == SDL_SCANCODE_S)
                    keys = 0;
                else if (code == SDL_SCANCODE_A)
                    keya = 0;
                else if (code == SDL_SCANCODE_D)
                    keyd = 0;
                else if (code == SDL_SCANCODE_UP)
                    arru = 0;
                else if (code == SDL_SCANCODE_RIGHT)
                    arrr = 0;
                else if (code == SDL_SCANCODE_DOWN)
                    arrd = 0;
                else if (code == SDL_SCANCODE_LEFT)
                    arrl = 0;
            }
        }
        SDL_RenderClear(ttrdr);
        if (delta < 4200)
            SDL_SetRenderDrawColor(ttrdr, 0, 0, 0,
                                   min(255, delta * 256 / 800));
        else {
            if (!roomchanged) {
                roomchanged         = 1;
                ttplayer.room       = ttmap + 'G';
                ttplayer.lenin_zhiv = 0;
                ttplayer.x          = 32 * 13;
                ttplayer.y          = 32 * 11;
            }
            SDL_SetRenderDrawColor(ttrdr, 0, 0, 0,
                                   255 - (delta - 4200) * 256 / 800);
        }
        tt_player_draw();
        SDL_Rect d = { 14, 14, 32 * 20, 32 * 16 };
        SDL_RenderFillRect(ttrdr, &d);
        if (delta < 4200) {
            SDL_Color    c    = { 128, 20, 20, min(255, delta * 256 / 800) };
            SDL_Surface *surf = TTF_RenderText_Blended(ttfont, gulagmsg, c);
            SDL_Texture *t    = SDL_CreateTextureFromSurface(ttrdr, surf);
            SDL_Rect     dst  = { 14, 14 + 32 * 7, surf->w * 2, surf->h * 2 };
            SDL_RenderCopy(ttrdr, t, 0, &dst);
            SDL_DestroyTexture(t);
            SDL_FreeSurface(surf);
        } else {
            SDL_Color c = { 128, 20, 20, 255 - (delta - 4200) * 256 / 800 };
            SDL_Surface *surf = TTF_RenderText_Blended(ttfont, gulagmsg, c);
            SDL_Texture *t    = SDL_CreateTextureFromSurface(ttrdr, surf);
            SDL_Rect     dst  = { 14, 14 + 32 * 7, surf->w * 2, surf->h * 2 };
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
    int           roomchanged = 0;
    SDL_BlendMode mode;
    SDL_GetRenderDrawBlendMode(ttrdr, &mode);
    int newticks = SDL_GetTicks();
    SDL_SetRenderDrawBlendMode(ttrdr, SDL_BLENDMODE_BLEND);
    while (!q && newticks < ticks + 1000) {
        int       delta = newticks - ticks;
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) q = 1;
            else if (e.type == SDL_KEYDOWN) {
                int code = e.key.keysym.scancode;
                if (code == SDL_SCANCODE_W) keyw = 1;
                else if (code == SDL_SCANCODE_S)
                    keys = 1;
                else if (code == SDL_SCANCODE_A)
                    keya = 1;
                else if (code == SDL_SCANCODE_D)
                    keyd = 1;
                else if (code == SDL_SCANCODE_UP)
                    arru = 1;
                else if (code == SDL_SCANCODE_RIGHT)
                    arrr = 1;
                else if (code == SDL_SCANCODE_DOWN)
                    arrd = 1;
                else if (code == SDL_SCANCODE_LEFT)
                    arrl = 1;
                else if (code == SDL_SCANCODE_F11) {
                    SDL_SetWindowFullscreen(
                        ttwdw, fullscreen ? 0 : SDL_WINDOW_FULLSCREEN);
                    fullscreen = !fullscreen;
                }
            } else if (e.type == SDL_KEYUP) {
                int code = e.key.keysym.scancode;
                if (code == SDL_SCANCODE_W) keyw = 0;
                else if (code == SDL_SCANCODE_S)
                    keys = 0;
                else if (code == SDL_SCANCODE_A)
                    keya = 0;
                else if (code == SDL_SCANCODE_D)
                    keyd = 0;
                else if (code == SDL_SCANCODE_UP)
                    arru = 0;
                else if (code == SDL_SCANCODE_RIGHT)
                    arrr = 0;
                else if (code == SDL_SCANCODE_DOWN)
                    arrd = 0;
                else if (code == SDL_SCANCODE_LEFT)
                    arrl = 0;
            }
        }
        SDL_RenderClear(ttrdr);
        if (delta < 500)
            SDL_SetRenderDrawColor(ttrdr, 0, 0, 0,
                                   min(255, delta * 256 / 500));
        else {
            if (!roomchanged) {
                roomchanged   = 1;
                ttplayer.room = ttmap + 'L';
                ttplayer.y    = TT_ROOM_H * 32 - 64;
            }
            SDL_SetRenderDrawColor(ttrdr, 0, 0, 0,
                                   255 - (delta - 500) * 256 / 500);
        }
        tt_player_draw();
        if (delta >= 500) {
            SDL_Rect src = { 96, 16 * 12, 64, 48 };
            SDL_Rect dst = { 14 + ttplayer.lenin_pos - 32, 110, 128, 96 };
            SDL_RenderCopy(ttrdr, tttxr, &src, &dst);
        }
        SDL_Rect d = { 14, 14, 32 * 20, 32 * 16 };
        SDL_RenderFillRect(ttrdr, &d);
        SDL_RenderPresent(ttrdr);
        newticks = SDL_GetTicks();
    }
    ticks    = newticks;
    newticks = SDL_GetTicks();
    Mix_PlayMusic(lenin, -1);
    curmus = lenin;
    while (!q && newticks < ticks + 14300) {
        int       delta = newticks - ticks;
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) q = 1;
            else if (e.type == SDL_KEYDOWN) {
                int code = e.key.keysym.scancode;
                if (code == SDL_SCANCODE_W) keyw = 1;
                else if (code == SDL_SCANCODE_S)
                    keys = 1;
                else if (code == SDL_SCANCODE_A)
                    keya = 1;
                else if (code == SDL_SCANCODE_D)
                    keyd = 1;
                else if (code == SDL_SCANCODE_UP)
                    arru = 1;
                else if (code == SDL_SCANCODE_RIGHT)
                    arrr = 1;
                else if (code == SDL_SCANCODE_DOWN)
                    arrd = 1;
                else if (code == SDL_SCANCODE_LEFT)
                    arrl = 1;
                else if (code == SDL_SCANCODE_F11) {
                    SDL_SetWindowFullscreen(
                        ttwdw, fullscreen ? 0 : SDL_WINDOW_FULLSCREEN);
                    fullscreen = !fullscreen;
                }
            } else if (e.type == SDL_KEYUP) {
                int code = e.key.keysym.scancode;
                if (code == SDL_SCANCODE_W) keyw = 0;
                else if (code == SDL_SCANCODE_S)
                    keys = 0;
                else if (code == SDL_SCANCODE_A)
                    keya = 0;
                else if (code == SDL_SCANCODE_D)
                    keyd = 0;
                else if (code == SDL_SCANCODE_UP)
                    arru = 0;
                else if (code == SDL_SCANCODE_RIGHT)
                    arrr = 0;
                else if (code == SDL_SCANCODE_DOWN)
                    arrd = 0;
                else if (code == SDL_SCANCODE_LEFT)
                    arrl = 0;
            }
        }
        SDL_RenderClear(ttrdr);
        tt_player_draw();
        {
            int      lenin_size = delta * (48 - 4 - 6) / 14300;
            SDL_Rect src        = { 32, 16 * 12 + 4, 32, lenin_size };
            SDL_Rect dst        = { 14 + ttplayer.lenin_pos,
                             14 + 32 + 96 - 8 - lenin_size * 2, 64,
                             lenin_size * 2 };
            SDL_RenderCopy(ttrdr, tttxr, &src, &dst);
        }
        {
            SDL_Rect src = { 96, 16 * 12, 64, 48 };
            SDL_Rect dst = { 14 + ttplayer.lenin_pos - 32, 110, 128, 96 };
            SDL_RenderCopy(ttrdr, tttxr, &src, &dst);
        }
        SDL_RenderPresent(ttrdr);
        newticks = SDL_GetTicks();
    }
    ttplayer.lenin_zhiv = 1;
    magic               = 0;
    ticks               = SDL_GetTicks();
}

void changeroom(int out)
{
    int newticks = SDL_GetTicks();
    while (!q && newticks < ticks + 500) {
        int       delta = newticks - ticks;
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) q = 1;
            else if (e.type == SDL_KEYDOWN) {
                int code = e.key.keysym.scancode;
                if (code == SDL_SCANCODE_W) keyw = 1;
                else if (code == SDL_SCANCODE_S)
                    keys = 1;
                else if (code == SDL_SCANCODE_A)
                    keya = 1;
                else if (code == SDL_SCANCODE_D)
                    keyd = 1;
                else if (code == SDL_SCANCODE_UP)
                    arru = 1;
                else if (code == SDL_SCANCODE_RIGHT)
                    arrr = 1;
                else if (code == SDL_SCANCODE_DOWN)
                    arrd = 1;
                else if (code == SDL_SCANCODE_LEFT)
                    arrl = 1;
                else if (code == SDL_SCANCODE_F11) {
                    SDL_SetWindowFullscreen(
                        ttwdw, fullscreen ? 0 : SDL_WINDOW_FULLSCREEN);
                    fullscreen = !fullscreen;
                }
            } else if (e.type == SDL_KEYUP) {
                int code = e.key.keysym.scancode;
                if (code == SDL_SCANCODE_W) keyw = 0;
                else if (code == SDL_SCANCODE_S)
                    keys = 0;
                else if (code == SDL_SCANCODE_A)
                    keya = 0;
                else if (code == SDL_SCANCODE_D)
                    keyd = 0;
                else if (code == SDL_SCANCODE_UP)
                    arru = 0;
                else if (code == SDL_SCANCODE_RIGHT)
                    arrr = 0;
                else if (code == SDL_SCANCODE_DOWN)
                    arrd = 0;
                else if (code == SDL_SCANCODE_LEFT)
                    arrl = 0;
            }
        }
        SDL_RenderClear(ttrdr);
        int tox    = (out == 3) - (out == 1);
        int toy    = (out == 0) - (out == 2);
        int transx = tox * (delta * TT_ROOM_W * 32 / 500);
        int transy = toy * (delta * TT_ROOM_H * 32 / 500);
        int playx  = tox * (delta * -32 / 500);
        int playy  = toy * (delta * -32 / 500);
        int fullx  = tox * TT_ROOM_W * -32;
        int fully  = toy * TT_ROOM_H * -32;
        {
            int      i, j;
            tt_room *r = ttplayer.room->neighbours[out];
            for (i = 0; i != TT_ROOM_H; ++i) {
                for (j = 0; j != TT_ROOM_W; ++j) {
                    SDL_Rect  d = { 14 + j * 32 + fullx + transx,
                                   14 + i * 32 + fully + transy, 32, 32 };
                    SDL_Rect *f = r->floor[i][j];
                    tt_body * w = r->walls[i][j];
                    if (f) SDL_RenderCopy(ttrdr, tttxr, f, &d);
                    if (w) {
                        SDL_Rect s = { (w->txrcol * w->anim +
                                        w->rem / w->rate % w->anim) *
                                           16,
                                       w->txrrow * 16, 16, 16 };
                        SDL_RenderCopy(ttrdr, tttxr, &s, &d);
                    }
                }
            }

            for (i = 0; i != r->bodies_count; ++i) {
                tt_body *b = r->bodies + i;
                SDL_Rect s = {
                    (b->txrcol * b->anim + b->rem / b->rate % b->anim) * 16,
                    b->txrrow * 16, 16, 16
                };
                SDL_Rect d = { 14 + b->x + fullx + transx,
                               14 + b->y + fully + transy, 32, 32 };
                SDL_RenderCopy(ttrdr, tttxr, &s, &d);
                if (b->msg) {
                    SDL_Color    c = { 255, 255, 255, 255 };
                    SDL_Surface *s =
                        TTF_RenderText_Blended(ttfont, b->msg, c);
                    SDL_Texture *t   = SDL_CreateTextureFromSurface(ttrdr, s);
                    SDL_Rect     dst = { 50 + b->x + fullx + transx,
                                     20 + b->y + fully + transy, s->w, s->h };
                    SDL_RenderCopy(ttrdr, t, 0, &dst);
                    SDL_DestroyTexture(t);
                    SDL_FreeSurface(s);
                }
            }
        }
        {
            int      i, j;
            tt_room *r = ttplayer.room;
            for (i = 0; i != TT_ROOM_H; ++i) {
                for (j = 0; j != TT_ROOM_W; ++j) {
                    SDL_Rect d = { 14 + j * 32 + transx, 14 + i * 32 + transy,
                                   32, 32 };
                    SDL_Rect *f = r->floor[i][j];
                    tt_body * w = r->walls[i][j];
                    if (f) SDL_RenderCopy(ttrdr, tttxr, f, &d);
                    if (w) {
                        SDL_Rect s = { (w->txrcol * w->anim +
                                        w->rem / w->rate % w->anim) *
                                           16,
                                       w->txrrow * 16, 16, 16 };
                        SDL_RenderCopy(ttrdr, tttxr, &s, &d);
                    }
                }
            }

            for (i = 0; i != r->bodies_count; ++i) {
                tt_body *b = r->bodies + i;
                SDL_Rect s = {
                    (b->txrcol * b->anim + b->rem / b->rate % b->anim) * 16,
                    b->txrrow * 16, 16, 16
                };
                SDL_Rect d = { 14 + b->x + transx, 14 + b->y + transy, 32,
                               32 };
                SDL_RenderCopy(ttrdr, tttxr, &s, &d);
                if (b->msg) {
                    SDL_Color    c = { 255, 255, 255, 255 };
                    SDL_Surface *s =
                        TTF_RenderText_Blended(ttfont, b->msg, c);
                    SDL_Texture *t = SDL_CreateTextureFromSurface(ttrdr, s);
                    SDL_Rect dst   = { 50 + b->x + transx, 20 + b->y + transy,
                                     s->w, s->h };
                    SDL_RenderCopy(ttrdr, t, 0, &dst);
                    SDL_DestroyTexture(t);
                    SDL_FreeSurface(s);
                }
            }

            SDL_Rect d   = { 14 + ttplayer.x + transx + playx,
                           14 + ttplayer.y + transy + playy, 32, 32 };
            int      dir = 0;
            if (ttplayer.xwalk == 1) dir = 6;
            else if (ttplayer.xwalk == -1)
                dir = 2;
            else if (ttplayer.ywalk == -1)
                dir = 4;
            SDL_Rect s = { 16 * (8 * ttplayer.variant + dir +
                                 (ttplayer.rem / 100 % 2)),
                           16 * 6, 16, 16 };
            SDL_RenderCopy(ttrdr, tttxr, &s, &d);
        }
        {
            SDL_Rect d = { 0, 0, 14, 540 };
            SDL_RenderFillRect(ttrdr, &d);
        }
        {
            SDL_Rect d = { 0, 0, 950, 14 };
            SDL_RenderFillRect(ttrdr, &d);
        }
        {
            SDL_Rect d = { 14 + TT_ROOM_W * 32, 0, 950, 540 };
            SDL_RenderFillRect(ttrdr, &d);
        }
        {
            SDL_Rect d = { 0, 14 + TT_ROOM_H * 32, 950, 540 };
            SDL_RenderFillRect(ttrdr, &d);
        }

        {
            SDL_Rect src = { 0, 16 * 11, 16, 16 };
            SDL_Rect d   = { 20 + TT_ROOM_W * 32 + 32, 12 + 32, 64, 64 };
            SDL_RenderCopy(ttrdr, tttxr, &src, &d);
            int count  = ttplayer.keys[0];
            int digits = 1;
            while (count / 10) {
                ++digits;
                count /= 10;
            }
            char *text = malloc(3 + digits);
            text[0]    = 'x';
            text[1]    = ' ';
            int i;
            count = ttplayer.keys[0];
            for (i = digits + 1; i != 1; --i) {
                text[i] = count % 10 + '0';
                count /= 10;
            }
            text[digits + 2] = 0;
            SDL_Color    c   = { 255, 255, 255, 255 };
            SDL_Surface *s   = TTF_RenderText_Blended(ttfont, text, c);
            SDL_Texture *t   = SDL_CreateTextureFromSurface(ttrdr, s);
            SDL_Rect     dst = { 50 + 32 * TT_ROOM_W + 64, 20 + 32, s->w * 2,
                             s->h * 2 };
            SDL_RenderCopy(ttrdr, t, 0, &dst);
            SDL_DestroyTexture(t);
            SDL_FreeSurface(s);
        }
        {
            SDL_Rect src = { 16, 16 * 11, 16, 16 };
            SDL_Rect d   = { 20 + TT_ROOM_W * 32 + 32, 12 + 96, 64, 64 };
            SDL_RenderCopy(ttrdr, tttxr, &src, &d);
            int count  = ttplayer.keys[1];
            int digits = 1;
            while (count / 10) {
                ++digits;
                count /= 10;
            }
            char *text = malloc(3 + digits);
            text[0]    = 'x';
            text[1]    = ' ';
            int i;
            count = ttplayer.keys[1];
            for (i = digits + 1; i != 1; --i) {
                text[i] = count % 10 + '0';
                count /= 10;
            }
            text[digits + 2] = 0;
            SDL_Color    c   = { 255, 255, 255, 255 };
            SDL_Surface *s   = TTF_RenderText_Blended(ttfont, text, c);
            SDL_Texture *t   = SDL_CreateTextureFromSurface(ttrdr, s);
            SDL_Rect     dst = { 50 + 32 * TT_ROOM_W + 64, 20 + 96, s->w * 2,
                             s->h * 2 };
            SDL_RenderCopy(ttrdr, t, 0, &dst);
            SDL_DestroyTexture(t);
            SDL_FreeSurface(s);
        }
        {
            int count  = ttplayer.money;
            int digits = 1;
            while (count / 10) {
                ++digits;
                count /= 10;
            }
            char *text = malloc(4 + digits);
            text[0]    = '$';
            text[1]    = ' ';
            text[2]    = ttplayer.money < 0 ? '-' : ' ';
            int i;
            count = ttplayer.money * (ttplayer.money < 0 ? -1 : 1);
            for (i = digits + 2; i != 2; --i) {
                text[i] = count % 10 + '0';
                count /= 10;
            }
            text[digits + 3] = 0;
            SDL_Color    c   = { 255, 255, 255, 255 };
            SDL_Surface *s   = TTF_RenderText_Blended(ttfont, text, c);
            SDL_Texture *t   = SDL_CreateTextureFromSurface(ttrdr, s);
            SDL_Rect dst = { 50 + 32 * TT_ROOM_W + 16, 20 + 96 + 96, s->w * 2,
                             s->h * 2 };
            SDL_RenderCopy(ttrdr, t, 0, &dst);
            SDL_DestroyTexture(t);
            SDL_FreeSurface(s);
        }
        if (ttplayer.lenin_rem) {
            int count  = ttplayer.lenin_rem / 1000;
            int digits = 1;
            while (count / 10) {
                ++digits;
                count /= 10;
            }
            char *text = malloc(7 + digits);
            text[0]    = 'T';
            text[1]    = 'i';
            text[2]    = 'm';
            text[3]    = 'e';
            text[4]    = ':';
            text[5]    = ' ';
            int i;
            count = ttplayer.lenin_rem / 1000;
            for (i = digits + 5; i != 5; --i) {
                text[i] = count % 10 + '0';
                count /= 10;
            }
            text[digits + 6] = 0;
            SDL_Color    c   = { 255, 255, 255, 255 };
            SDL_Surface *s   = TTF_RenderText_Blended(ttfont, text, c);
            SDL_Texture *t   = SDL_CreateTextureFromSurface(ttrdr, s);
            SDL_Rect     dst = { 50 + 32 * TT_ROOM_W + 16, 20 + 96 + 96 + 96,
                             s->w * 2, s->h * 2 };
            SDL_RenderCopy(ttrdr, t, 0, &dst);
            SDL_DestroyTexture(t);
            SDL_FreeSurface(s);
        }

        SDL_RenderPresent(ttrdr);
        newticks = SDL_GetTicks();
    }
    if (out == 0) ttplayer.y = TT_ROOM_H * 32 - 32;
    else if (out == 1)
        ttplayer.x = 0;
    else if (out == 2)
        ttplayer.y = 0;
    else if (out == 3)
        ttplayer.x = TT_ROOM_W * 32 - 32;
    ttplayer.room = ttplayer.room->neighbours[out];
    ticks         = SDL_GetTicks();
    magic         = 0;
}

void tt_mainloop()
{
    keyw = keya = keys = keyd = arru = arrr = arrd = arrl = 0;
    ticks                                                 = SDL_GetTicks();
    while (!q) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) q = 1;
            else if (e.type == SDL_KEYDOWN) {
                int code = e.key.keysym.scancode;
                if (code == SDL_SCANCODE_W) keyw = 1;
                else if (code == SDL_SCANCODE_S)
                    keys = 1;
                else if (code == SDL_SCANCODE_A)
                    keya = 1;
                else if (code == SDL_SCANCODE_D)
                    keyd = 1;
                else if (code == SDL_SCANCODE_UP)
                    arru = 1;
                else if (code == SDL_SCANCODE_RIGHT)
                    arrr = 1;
                else if (code == SDL_SCANCODE_DOWN)
                    arrd = 1;
                else if (code == SDL_SCANCODE_LEFT)
                    arrl = 1;
                else if (code == SDL_SCANCODE_F5)
                    save();
                else if (code == SDL_SCANCODE_F6)
                    load();
                else if (code == SDL_SCANCODE_F11) {
                    SDL_SetWindowFullscreen(
                        ttwdw, fullscreen ? 0 : SDL_WINDOW_FULLSCREEN);
                    fullscreen = !fullscreen;
                }
            } else if (e.type == SDL_KEYUP) {
                int code = e.key.keysym.scancode;
                if (code == SDL_SCANCODE_W) keyw = 0;
                else if (code == SDL_SCANCODE_S)
                    keys = 0;
                else if (code == SDL_SCANCODE_A)
                    keya = 0;
                else if (code == SDL_SCANCODE_D)
                    keyd = 0;
                else if (code == SDL_SCANCODE_UP)
                    arru = 0;
                else if (code == SDL_SCANCODE_RIGHT)
                    arrr = 0;
                else if (code == SDL_SCANCODE_DOWN)
                    arrd = 0;
                else if (code == SDL_SCANCODE_LEFT)
                    arrl = 0;
            }
        }
        ttplayer.ywalk = (keys | arrd) - (keyw | arru);
        ttplayer.xwalk = (keyd | arrr) - (keya | arrl);
        int newticks   = SDL_GetTicks();
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
        } else if (magic >= tt_changeroom) {
            changeroom(magic - tt_changeroom);
        }
    }
}
