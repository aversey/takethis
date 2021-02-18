#include <SDL2/SDL.h>
#include "utils.h"


enum { shape_size = 16 };


int main(int argc, char **argv)
{
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* background;
    SDL_Texture* shape;

    SDL_Rect src;
    SDL_Rect dst;

    src.x = 0;
    src.y = 0;
    src.w = shape_size;
    src.h = shape_size;

    dst.x = 640 / 2 - shape_size;
    dst.y = 480 / 2 - shape_size;
    dst.w = shape_size * 2;
    dst.h = shape_size * 2;

    window = SDL_CreateWindow("T A K E T H I S",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              640, 480, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    background = load_texture(renderer, "data/background.bmp");
    shape = load_texture(renderer, "data/shape.bmp");

    int i;
    int n;
    for (i = 0; i < 2; ++i) {
        for(n = 0; n < 4; ++n) {
            src.x = shape_size * (n % 2);
            if (n > 1) {
                src.y = shape_size;
            } else {
                src.y = 0;
            }
            SDL_RenderCopy(renderer, background, NULL, NULL);
            SDL_RenderCopy(renderer, shape, &src, &dst);
            SDL_RenderPresent(renderer);
            SDL_Delay(500);
        }
    }

    SDL_DestroyTexture(shape);
    SDL_DestroyTexture(background);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
