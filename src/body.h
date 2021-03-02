#ifndef TT_INCLUDED_BODY
#define TT_INCLUDED_BODY

enum {
    colact_grib = 1,
    colact_gulag,
    colact_instgulag,
    colact_key,
    colact_door
};

typedef struct tt_body {
    int   x;
    int   y;
    int   xrem;
    int   yrem;
    int   xvel;
    int   yvel;
    int   txrrow;
    int   txrcol;
    int   rem;
    int   anim;
    int   rate;
    int   collision_act;
    char *msg;
    int   msglen;
    int   isdoor;
} tt_body;

void tt_body_move(int d);

#endif
