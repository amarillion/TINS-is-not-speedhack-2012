#ifndef _PARALLAX_
#define _PARALLAX_
typedef struct Parallax Parallax;
#include "common.h"
#include "main.h"
struct Parallax {
    LandImage * sky;
    LandImage * ground;
    LandArray * layers[4];
};
void parallax_init(void);
void parallax_draw(void);
#endif
