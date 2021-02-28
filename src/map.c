#include "map.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include "room.h"
#include "globals.h"
#include "game.h"


static int readnum(FILE *f)
{
    int neg = fgetc(f) == '-';
    int num = fgetc(f) - '0';
    num *= 10;
    num += fgetc(f) - '0';
    num *= 10;
    num += fgetc(f) - '0';
    return (neg ? -1 : 1) * num;
}

static SDL_Rect *newtile(int row, int col)
{
    SDL_Rect *r = malloc(sizeof(*r));
    r->x = 16 * col;
    r->y = 16 * row;
    r->h = r->w = 16;
    return r;
}

static void do_nothing(tt_body *b)
{}

static void gribtake(tt_body *b)
{
    Mix_PlayMusic(ponpon, -1);
    b->collision_act = do_nothing;
    b->anim = 1;
    b->txrrow = 0;
    b->txrcol = 15;
    magic = tt_gotofirstroom;
}

static void loadroom(tt_room *r, FILE *f)
{
    int default_floor_id = fgetc(f) - '0';
    fgetc(f);
    int i, j;
    for (i = 0; i != 4; ++i) r->neighbours[i] = ttmap + fgetc(f);
    r->bodies = 0;
    r->bodies_count = 0;
    fgetc(f);
    for (i = 0; i != TT_ROOM_H; ++i) {
        for (j = 0; j != TT_ROOM_W; ++j) {
            int type = fgetc(f);
            int id   = fgetc(f);
            int c    = id;
            if (id == ' ') id = '0';
            id -= '0';
            r->floor[i][j] = 0;
            r->walls[i][j] = 0;
            r->roof[i][j]  = 0;
            if (type == '@') {
                ttplayer.room = r;
                ttplayer.x = j * 32;
                ttplayer.y = i * 32;
                ttplayer.xrem = 0;
                ttplayer.yrem = 0;
                ttplayer.xwalk = 0;
                ttplayer.ywalk = 0;
                ttplayer.rem = 0;
                ttplayer.money = 0;
                ttplayer.keys = 0;
                ttplayer.the_key = 0;
                ttplayer.variant = 1;
                r->floor[i][j] = newtile(0, default_floor_id);
            } else if (type == '#') {
                r->walls[i][j] = malloc(sizeof(tt_body));
                tt_body *b = r->walls[i][j];
                b->x = j * 32;
                b->y = i * 32;
                b->xrem = 0;
                b->yrem = 0;
                b->xvel = 0;
                b->yvel = 0;
                b->rem = 0;
                b->txrrow = 1;
                b->txrcol = id;
                b->anim = 1;
                b->rate = 1;
                b->collision_act = do_nothing;
                b->msg = 0;
                b->msglen = 0;
            } else if (type == 'M') {
                r->floor[i][j] = newtile(0, default_floor_id);
                r->bodies_count++;
                r->bodies = realloc(r->bodies,
                                    sizeof(tt_body) * r->bodies_count);
                tt_body *b = r->bodies + r->bodies_count - 1;
                b->x = j * 32;
                b->y = i * 32;
                b->xrem = 0;
                b->yrem = 0;
                b->xvel = 0;
                b->yvel = 0;
                b->rem = 0;
                b->txrrow = 8;
                b->txrcol = id;
                b->anim = 4;
                b->rate = 150 + (rand() % 50 - 25);
                b->collision_act = gribtake;
                b->msg = 0;
                b->msglen = 0;
            } else if (type == '^') {
                r->floor[i][j] = newtile(0, default_floor_id);
                r->walls[i][j] = malloc(sizeof(tt_body));
                tt_body *b = r->walls[i][j];
                b->x = j * 32;
                b->y = i * 32;
                b->xrem = 0;
                b->yrem = 0;
                b->xvel = 0;
                b->yvel = 0;
                b->rem = 0;
                b->txrrow = 8;
                b->txrcol = id;
                b->anim = 4;
                b->rate = 150 + (rand() % 50 - 25);
                b->collision_act = do_nothing;
                b->msg = 0;
                b->msglen = 0;
            } else if (type == '&') {
                r->floor[i][j] = newtile(0, default_floor_id);
                r->walls[i][j] = malloc(sizeof(tt_body));
                tt_body *b = r->walls[i][j];
                b->x = j * 32;
                b->y = i * 32;
                b->xrem = 0;
                b->yrem = 0;
                b->xvel = 0;
                b->yvel = 0;
                b->rem = 0;
                b->txrrow = 10;
                b->txrcol = id;
                b->anim = 1;
                b->rate = 100;
                b->collision_act = do_nothing;
                b->msg = 0;
                b->msglen = 0;
            } else if (type == '.') {
                r->floor[i][j] = newtile(0, c == ' ' ? default_floor_id : id);
            } else if (type == '*') {
                r->walls[i][j] = malloc(sizeof(tt_body));
                tt_body *b = r->walls[i][j];
                b->x = j * 32;
                b->y = i * 32;
                b->xrem = 0;
                b->yrem = 0;
                b->xvel = 0;
                b->yvel = 0;
                b->rem = 0;
                b->txrrow = 9;
                b->txrcol = id;
                b->anim = 4;
                b->rate = 150 + (rand() % 50 - 25);
                b->collision_act = do_nothing;
                b->msg = 0;
                b->msglen = 0;
            } else if (type == 'X') {
                r->floor[i][j] = newtile(0, default_floor_id);
                r->roof[i][j] = newtile(5, id);
            } else if (type == '=') {
                r->floor[i][j] = newtile(0, default_floor_id);
                r->bodies_count++;
                r->bodies = realloc(r->bodies,
                                    sizeof(tt_body) * r->bodies_count);
                tt_body *b = r->bodies + r->bodies_count - 1;
                b->x = j * 32;
                b->y = i * 32;
                b->xrem = 0;
                b->yrem = 0;
                b->rem = 0;
                b->txrrow = 0;
                b->txrcol = 15;
                b->anim = 1;
                b->rate = 100;
                b->collision_act = do_nothing;
            } else if (type == '$') {
                r->floor[i][j] = newtile(0, default_floor_id);
                r->bodies_count++;
                r->bodies = realloc(r->bodies,
                                    sizeof(tt_body) * r->bodies_count);
                tt_body *b = r->bodies + r->bodies_count - 1;
                b->x = j * 32;
                b->y = i * 32;
                b->xrem = 0;
                b->yrem = 0;
                b->rem = 0;
                b->txrrow = 7;
                b->txrcol = id;
                b->anim = 4;
                b->rate = 100 + (rand() % 50 - 25);
                b->collision_act = 0; /* todo: kill */
            }
        }
        fgetc(f);
    }
    for (i = 0; i != r->bodies_count; ++i) {
        r->bodies[i].yvel = readnum(f);
        r->bodies[i].xvel = readnum(f);
        r->bodies[i].msg = 0;
        r->bodies[i].msglen = 0;
        if (fgetc(f) == '=') {
            int c = fgetc(f);
            while (c != EOF && c != '\n') {
                r->bodies[i].msglen++;
                r->bodies[i].msg = realloc(r->bodies[i].msg,
                                            r->bodies[i].msglen + 1);
                r->bodies[i].msg[r->bodies[i].msglen - 1] = c;
                c = fgetc(f);
            }
            r->bodies[i].msg[r->bodies[i].msglen] = 0;
        }
    }
}

void tt_map_load()
{
    char path[] = "data/map/0";
    char r = '0';
    while (r != '~') {
        path[9] = r;
        FILE *roomf = fopen(path, "r");
        if (roomf) {
            loadroom(ttmap + r, roomf);
            fclose(roomf);
        }
        ++r;
    }
}


void tt_map_free()
{
    return;
}
