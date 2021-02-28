#ifndef TT_INCLUDED_BODY
#define TT_INCLUDED_BODY


typedef struct tt_body {
    int x;
    int y;
    int xrem;
    int yrem;
    int xvel;
    int yvel;
    int txrrow;
    int txrcol;
    int rem;
    int anim;
    int rate;
    void (*collision_act)(struct tt_body *b);
    char *msg;
    int msglen;
} tt_body;


void tt_body_move(int d);


#endif
