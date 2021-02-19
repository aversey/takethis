#include "room.h"

#include <stdio.h>
#include "texture.h"


#define ROOM_W 19
#define ROOM_H 15


static char *readline(FILE *f)
{
    int len = 0;
    char *res = malloc(1);
    int c = fgetc(f);
    while (c != EOF && c != '\n') {
        res = realloc(res, len + 2);
        res[len++] = c;
        c = fgetc(f);
    }
    res[len] = 0;
    return res;
}

static char skipspaces(FILE *f)
{
    int c = fgetc(f);
    while (c == ' ' || c == '\n' || c == '\t') c = fgetc(f);
    return c == EOF ? 0 : c;
}

tt_room *tt_room_load(int num)
{
    tt_room *res = malloc(sizeof(*res));
    char path[] = "data/rooms/00";
    path[11] = '0' + num / 10;
    path[12] = '0' + num % 10;
    FILE *f = fopen(path, "r");
    char *type = readline(f);
    free(type);
    int i, j;
    for (i = 0; i != ROOM_H; ++i) {
        for (j = 0; j != ROOM_W; ++j) {
            char c = skipspaces(f);
            if (c == '.') {
                res->tiles[i][j] = &tt_texture_floor;
                res->bodies[i][j] = 0;
            } else if (c == '#') {
                if (i == 0 && j == 0) {
                    res->tiles[i][j] = &tt_texture_corner_lu;
                } else if (i == 0 && j == ROOM_W - 1) {
                    res->tiles[i][j] = &tt_texture_corner_ru;
                } else if (i == ROOM_H - 1 && j == 0) {
                    res->tiles[i][j] = &tt_texture_corner_ld;
                } else if (i == ROOM_H - 1 && j == ROOM_W - 1) {
                    res->tiles[i][j] = &tt_texture_corner_rd;
                } else if (i == 0) {
                    res->tiles[i][j] = &tt_texture_wall_u;
                } else if (i == ROOM_H - 1) {
                    res->tiles[i][j] = &tt_texture_wall_d;
                } else if (j == 0) {
                    res->tiles[i][j] = &tt_texture_wall_l;
                } else if (j == ROOM_W - 1) {
                    res->tiles[i][j] = &tt_texture_wall_r;
                } else {
                    fclose(f);
                    free(res);
                    return 0;
                }
                res->bodies[i][j] = 0;
/*            } else if (c == '@') {
                res->tiles[i][j] = tt_tile_floor;
                res->bodies[i][j] = tt_player_spawn();
                if (!res->bodies[i][j]) {
                    flcose(f);
                    free(res);
                    return 0;
                } */
            } else {
                fclose(f);
                free(res);
                return 0;
            }
        }
    }
    return res;
}


void tt_room_draw(SDL_Renderer *rdr, SDL_Texture *txr, tt_room *room)
{
    int i, j;
    for (i = 0; i != ROOM_H; ++i) {
        for (j = 0; j != ROOM_W; ++j) {
            const SDL_Rect *src = room->tiles[i][j];
            if (src) {
                SDL_Rect dst = { 30 + 32 * j, 30 + 32 * i, 32, 32 };
                SDL_RenderCopy(rdr, txr, src, &dst);
            }
        }
    }
}
