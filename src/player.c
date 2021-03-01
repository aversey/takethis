#include "player.h"

#include <math.h>
#include "globals.h"


void tt_player_draw()
{
    int i, j;
    tt_room *r = ttplayer.room;
    for (i = 0; i != TT_ROOM_H; ++i) {
        for (j = 0; j != TT_ROOM_W; ++j) {
            SDL_Rect d = { 14 + j * 32, 14 + i * 32, 32, 32 };
            SDL_Rect *f = r->floor[i][j];
            tt_body *w = r->walls[i][j];
            if (f) SDL_RenderCopy(ttrdr, tttxr, f, &d);
            if (w) {
                SDL_Rect s = {
                    (w->txrcol * w->anim + w->rem / w->rate % w->anim) * 16,
                    w->txrrow * 16, 16, 16
                };
                SDL_RenderCopy(ttrdr, tttxr, &s, &d);
            }
        }
    }

    if (ttplayer.zhiv_lenin) {
        {
            SDL_Rect src = { 32 + 32 * (ttplayer.lenin_rem / 200 % 2),
                             16 * 12 + 4, 32, 38 };
            SDL_Rect dst = { 14 + ttplayer.lenin_pos,
                             14 + 32 + 96 - 8 - 38 * 2, 64, 38 * 2 };
            SDL_RenderCopy(ttrdr, tttxr, &src, &dst);
        }
        {
            SDL_Rect src = { 96 + 64 * (ttplayer.lenin_rem / 200 % 2),
                             16 * 12, 64, 48 };
            SDL_Rect dst = { 14 + ttplayer.lenin_pos - 32,
                             110, 128, 96 };
            SDL_RenderCopy(ttrdr, tttxr, &src, &dst);
        }
    }

    for (i = 0; i != r->bodies_count; ++i) {
        tt_body *b = r->bodies + i;
        SDL_Rect s = {
            (b->txrcol * b->anim + b->rem / b->rate % b->anim) * 16,
            b->txrrow * 16, 16, 16
        };
        SDL_Rect d = { 14 + b->x, 14 + b->y, 32, 32 };
        SDL_RenderCopy(ttrdr, tttxr, &s, &d);
        if (b->msg) {
            SDL_Color c = { 255, 255, 255, 255 };
            SDL_Surface *s = TTF_RenderText_Blended(ttfont, b->msg, c);
            SDL_Texture *t = SDL_CreateTextureFromSurface(ttrdr, s);
            SDL_Rect dst = { 50 + b->x, 20 + b->y, s->w, s->h };
            SDL_RenderCopy(ttrdr, t, 0, &dst);
            SDL_DestroyTexture(t);
            SDL_FreeSurface(s);
        }
    }

    SDL_Rect d = { 14 + ttplayer.x, 14 + ttplayer.y, 32, 32 };
    int dir = 0;
    if (ttplayer.xwalk == 1) dir = 6;
    else if (ttplayer.xwalk == -1) dir = 2;
    else if (ttplayer.ywalk == -1) dir = 4;
    SDL_Rect s = { 16 * (8 * ttplayer.variant +
                         dir + (ttplayer.rem / 100 % 2)),
                   16 * 6,
                   16, 16 };
    SDL_RenderCopy(ttrdr, tttxr, &s, &d);

    if (ttplayer.until_gulag > 0) {
        double r = ttplayer.until_gulag / 10;
        double f = (double)(ttplayer.until_gulag) / 3000 *
                   2 * 3.14159265358979323846;
        SDL_Rect s = { 0, 16 * 12, 32, 48 };
        SDL_Rect d = { ttplayer.x - 16 + r * cos(f),
                       ttplayer.y - 32 + r * sin(f),
                       64, 96 };
        SDL_RenderCopy(ttrdr, tttxr, &s, &d);
    }
}
