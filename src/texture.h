#ifndef TAKETHIS_INCLUDED_TEXTURE
#define TAKETHIS_INCLUDED_TEXTURE


#include <SDL2/SDL.h>


extern const SDL_Rect tt_texture_floor;
extern const SDL_Rect tt_texture_corner_lu;
extern const SDL_Rect tt_texture_corner_ru;
extern const SDL_Rect tt_texture_corner_ld;
extern const SDL_Rect tt_texture_corner_rd;
extern const SDL_Rect tt_texture_wall_l;
extern const SDL_Rect tt_texture_wall_r;
extern const SDL_Rect tt_texture_wall_u;
extern const SDL_Rect tt_texture_wall_d;
extern const SDL_Rect tt_texture_player;


SDL_Texture *tt_texture_load(SDL_Renderer *rdr);


#endif
