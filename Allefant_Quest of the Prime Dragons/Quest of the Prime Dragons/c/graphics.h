#ifndef _GRAPHICS_
#define _GRAPHICS_
#include "common.h"
#include "object.h"
#include "color.h"
Raytracer * graphics_parse(char const * script, int w, int h, Land4x4Matrix matrix, double dt);
#endif
