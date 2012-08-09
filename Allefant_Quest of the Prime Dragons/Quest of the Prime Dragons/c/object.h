#ifndef _OBJECT_
#define _OBJECT_
typedef struct Object Object;
#include "raytrace.h"
struct Object {
    LandArray * v;
    LandArray * soup;
    float r, g, b, a;
};
void object_rgba(Object * o, float r, float g, float b, float a);
#define V(i) ( * ((Vector * ) o->v->data[i]))
void object_recalc_normals(Object * o);
Object * object_new(void);
Object * object_sphere(int segments, int slices);
Object * object_cylinder_segments(int segments, bool top, bool bottom, int seg0, int seg1);
Object * object_cylinder(int segments, bool top, bool bottom);
Object * object_cone(int segments, bool bottom);
Object * object_box(void);
Object * object_pyramid(void);
Object * object_triangle(LandVector a, LandVector b, LandVector c);
void object_transform(Object * o, Land4x4Matrix * matrix);
void object_draw_lines(Object * o);
#endif
