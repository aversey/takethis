#include "player.h"

#include "globals.h"
#include <math.h>

void tt_player_draw()
{
    int      i, j;
    tt_room *r = ttplayer.room;
    for (i = 0; i != TT_ROOM_H; ++i) {
        for (j = 0; j != TT_ROOM_W; ++j) {
            SDL_Rect  d = {14 + j * 32, 14 + i * 32, 32, 32};
            SDL_Rect *f = r->floor[i][j];
            tt_body * w = r->walls[i][j];
            if (f) SDL_RenderCopy(ttrdr, tttxr, f, &d);
            if (w) {
                SDL_Rect s = {
                    (w->txrcol * w->anim + w->rem / w->rate % w->anim) * 16,
                    w->txrrow * 16, 16, 16};
                SDL_RenderCopy(ttrdr, tttxr, &s, &d);
            }
        }
    }

    if (ttplayer.lenin_zhiv) {
        {
            SDL_Rect src = {32 + 32 * (ttplayer.lenin_rem / 200 % 2),
                            16 * 12 + 4, 32, 38};
            SDL_Rect dst = {14 + ttplayer.lenin_pos, 14 + 32 + 96 - 8 - 38 * 2,
                            64, 38 * 2};
            SDL_RenderCopy(ttrdr, tttxr, &src, &dst);
        }
        {
            SDL_Rect src = {96 + 64 * (ttplayer.lenin_rem / 200 % 2), 16 * 12,
                            64, 48};
            SDL_Rect dst = {14 + ttplayer.lenin_pos - 32, 110, 128, 96};
            SDL_RenderCopy(ttrdr, tttxr, &src, &dst);
        }
    }

    for (i = 0; i != r->bodies_count; ++i) {
        tt_body *b = r->bodies + i;
        SDL_Rect s = {(b->txrcol * b->anim + b->rem / b->rate % b->anim) * 16,
                      b->txrrow * 16, 16, 16};
        if (b->isdoor) {
            SDL_Rect d = {14 + b->x, 14 + b->y, 32, 32};
            SDL_RenderCopy(ttrdr, tttxr, &s, &d);
        } else {
            SDL_Rect d = {14 + b->x, 14 + b->y, 32, 32};
            SDL_RenderCopy(ttrdr, tttxr, &s, &d);
        }
        if (b->msg) {
            SDL_Color    c   = {255, 255, 255, 255};
            SDL_Surface *s   = TTF_RenderText_Blended(ttfont, b->msg, c);
            SDL_Texture *t   = SDL_CreateTextureFromSurface(ttrdr, s);
            SDL_Rect     dst = {50 + b->x, 20 + b->y, s->w, s->h};
            SDL_RenderCopy(ttrdr, t, 0, &dst);
            SDL_DestroyTexture(t);
            SDL_FreeSurface(s);
        }
    }

    SDL_Rect d   = {14 + ttplayer.x, 14 + ttplayer.y, 32, 32};
    int      dir = 0;
    if (ttplayer.xwalk == 1) dir = 6;
    else if (ttplayer.xwalk == -1)
        dir = 2;
    else if (ttplayer.ywalk == -1)
        dir = 4;
    SDL_Rect s = {16 * (8 * ttplayer.variant + dir + (ttplayer.rem / 100 % 2)),
                  16 * 6, 16, 16};
    SDL_RenderCopy(ttrdr, tttxr, &s, &d);

    if (ttplayer.until_gulag > 0) {
        double r = ttplayer.until_gulag / 10;
        double f =
            (double)(ttplayer.until_gulag) / 3000 * 2 * 3.14159265358979323846;
        SDL_Rect s = {0, 16 * 12, 32, 48};
        SDL_Rect d = {ttplayer.x - 16 + r * cos(f),
                      ttplayer.y - 32 + r * sin(f), 64, 96};
        SDL_RenderCopy(ttrdr, tttxr, &s, &d);
    }

    {
        SDL_Rect src = {0, 16 * 11, 16, 16};
        SDL_Rect d   = {20 + TT_ROOM_W * 32 + 32, 12 + 32, 64, 64};
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
        SDL_Color    c   = {255, 255, 255, 255};
        SDL_Surface *s   = TTF_RenderText_Blended(ttfont, text, c);
        SDL_Texture *t   = SDL_CreateTextureFromSurface(ttrdr, s);
        SDL_Rect dst = {50 + 32 * TT_ROOM_W + 64, 20 + 32, s->w * 2, s->h * 2};
        SDL_RenderCopy(ttrdr, t, 0, &dst);
        SDL_DestroyTexture(t);
        SDL_FreeSurface(s);
    }
    {
        SDL_Rect src = {16, 16 * 11, 16, 16};
        SDL_Rect d   = {20 + TT_ROOM_W * 32 + 32, 12 + 96, 64, 64};
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
        SDL_Color    c   = {255, 255, 255, 255};
        SDL_Surface *s   = TTF_RenderText_Blended(ttfont, text, c);
        SDL_Texture *t   = SDL_CreateTextureFromSurface(ttrdr, s);
        SDL_Rect dst = {50 + 32 * TT_ROOM_W + 64, 20 + 96, s->w * 2, s->h * 2};
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
        SDL_Color    c   = {255, 255, 255, 255};
        SDL_Surface *s   = TTF_RenderText_Blended(ttfont, text, c);
        SDL_Texture *t   = SDL_CreateTextureFromSurface(ttrdr, s);
        SDL_Rect     dst = {50 + 32 * TT_ROOM_W + 16, 20 + 96 + 96, s->w * 2,
                        s->h * 2};
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
        SDL_Color    c   = {255, 255, 255, 255};
        SDL_Surface *s   = TTF_RenderText_Blended(ttfont, text, c);
        SDL_Texture *t   = SDL_CreateTextureFromSurface(ttrdr, s);
        SDL_Rect dst = {50 + 32 * TT_ROOM_W + 16, 20 + 96 + 96 + 96, s->w * 2,
                        s->h * 2};
        SDL_RenderCopy(ttrdr, t, 0, &dst);
        SDL_DestroyTexture(t);
        SDL_FreeSurface(s);
    }
}
