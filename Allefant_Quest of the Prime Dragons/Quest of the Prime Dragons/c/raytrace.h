#ifndef _RAYTRACE_
#define _RAYTRACE_
typedef struct Triangle Triangle;
typedef struct Ray Ray;
typedef struct Raytracer Raytracer;
#include "common.h"
#include "object.h"
typedef LandVector Vector;
typedef LandFloat Float;
struct Triangle {
    Vector a, b, c;
    Vector n;
    Object * o;
};
struct Ray {
    Vector a, u;
};
struct Raytracer {
    int w, h;
    Vector light;
    LandArray * objects;
    LandArray * * tiles;
    unsigned char * rgba;
};
Raytracer * raytracer_new(int w, int h);
void raytracer_add_object(Raytracer * self, Object * o);
void raytracer_add_light(Raytracer * self, Vector v);
void minmax3(float a, float b, float c, float * vmin, float * vmax);
unsigned char * raytracer_trace(Raytracer * self);
void raytracer_draw_lines(Raytracer * r);
#endif
