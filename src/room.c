#include "room.h"

#include <stdio.h>
#include "texture.h"


static SDL_Rect *rect(int x, int y, int w, int h)
{
    SDL_Rect *res = malloc(sizeof(*res));
    res->x = x;
    res->y = y;
    res->w = w;
    res->h = h;
    return res;
}

tt_room *tt_room_load(char c)
{
    tt_room *res = malloc(sizeof(*res));
    char path[] = "data/rooms/0";
    path[11] = c;
    FILE *f = fopen(path, "r");
    int i, j;
    for (i = 0; i != TT_ROOM_H; ++i) {
        for (j = 0; j != TT_ROOM_W; ++j) {
            int type = fgetc(f);
            res->tiletypes[i][j] = type;
            res->bodies[i][j] = 0;
            int id = fgetc(f);
            if (id == ' ') id = '0';
            if (id < '0' || '9' < id) goto error;
            id = (id - '0') * 16;
            if (type == '.')      res->tiles[i][j] = rect(id, 16, 16, 16);
            else if (type == '#') res->tiles[i][j] = rect(id, 0,  16, 16);
            else goto error;
        }
        fgetc(f);
    }
    fclose(f);
    return res;
error:
    fclose(f);
    free(res);
    return 0;
}


void tt_room_draw(SDL_Renderer *rdr, SDL_Texture *txr, tt_room *room)
{
    int i, j;
    for (i = 0; i != TT_ROOM_H; ++i) {
        for (j = 0; j != TT_ROOM_W; ++j) {
            const SDL_Rect *src = room->tiles[i][j];
            if (src) {
                SDL_Rect dst = { 30 + 32 * j, 30 + 32 * i, 32, 32 };
                SDL_RenderCopy(rdr, txr, src, &dst);
            }
        }
    }
}
