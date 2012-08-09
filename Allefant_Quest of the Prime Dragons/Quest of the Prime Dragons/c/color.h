#ifndef _COLOR_
#define _COLOR_
typedef struct Color Color;
#include "common.h"
struct Color {
    char const * name;
    unsigned char r, g, b;
};
extern Color colors[];
#endif
