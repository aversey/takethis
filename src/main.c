#include <SDL2/SDL.h>
#include "room.h"
#include "texture.h"


int main(int argc, char **argv)
{
    SDL_Window *wdw = SDL_CreateWindow("T A K E T H I S",
                                       SDL_WINDOWPOS_UNDEFINED,
                                       SDL_WINDOWPOS_UNDEFINED,
                                       950, 540, 0);
    SDL_Renderer *rdr = SDL_CreateRenderer(wdw, -1,
                                           SDL_RENDERER_ACCELERATED);
    SDL_Texture *txr = tt_texture_load(rdr);
    tt_room *room = tt_room_load('0');

    SDL_SetRenderDrawColor(rdr, 215, 174, 0, 255);

    int n;
    for (n = 0; n != 200; ++n) {
        SDL_RenderClear(rdr);
        tt_room_draw(rdr, txr, room);
        SDL_RenderPresent(rdr);
        SDL_Delay(25);
    }

    SDL_DestroyTexture(txr);
    SDL_DestroyRenderer(rdr);
    SDL_DestroyWindow(wdw);
    SDL_Quit();
    return 0;
}
