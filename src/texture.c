#include "texture.h"


const SDL_Rect tt_texture_floor     = { 16 * 7, 0,  16, 16 };
const SDL_Rect tt_texture_corner_lu = { 16 * 5, 0,  16, 16 };
const SDL_Rect tt_texture_corner_ru = { 16 * 6, 0,  16, 16 };
const SDL_Rect tt_texture_corner_ld = { 16 * 5, 16, 16, 16 };
const SDL_Rect tt_texture_corner_rd = { 16 * 6, 16, 16, 16 };
const SDL_Rect tt_texture_wall_l    = { 16 * 8, 0,  16, 16 };
const SDL_Rect tt_texture_wall_r    = { 16 * 8, 16, 16, 16 };
const SDL_Rect tt_texture_wall_u    = { 16 * 9, 0,  16, 16 };
const SDL_Rect tt_texture_wall_d    = { 16 * 9, 16, 16, 16 };
const SDL_Rect tt_texture_player    = { 0,      0,  0,  0 };


SDL_Texture *tt_texture_load(SDL_Renderer *rdr)
{
    SDL_Surface *surf = SDL_LoadBMP("data/tiles.bmp");
    SDL_Texture *res = SDL_CreateTextureFromSurface(rdr, surf);
    SDL_FreeSurface(surf);
    return res;
}
