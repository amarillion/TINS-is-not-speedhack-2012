#include "object.h"
static void add_vertex(Object * o, Float x, Float y, Float z) {
    Vector * v;
    land_alloc(v);
    v->x = x;
    v->y = y;
    v->z = z;
    land_array_add(o->v, v);
}
void object_rgba(Object * o, float r, float g, float b, float a) {
    o->r = r;
    o->g = g;
    o->b = b;
    o->a = a;
}
void object_recalc_normals(Object * o) {
    {LandArrayIterator __iter0__ = LandArrayIterator_first(o->soup) ; for (Triangle * t = LandArrayIterator_item(o->soup, & __iter0__) ; LandArrayIterator_next(o->soup, & __iter0__) ; t = LandArrayIterator_item(o->soup, & __iter0__)) {
        t->n = land_vector_normalize(land_vector_cross(land_vector_sub(t->b, t->a) , land_vector_sub(t->c, t->a)));
    }
    }
}
static void add_triangle(Object * o, int a, int b, int c) {
    Triangle * t;
    land_alloc(t);
    t->o = o;
    t->a = V(a);
    t->b = V(b);
    t->c = V(c);
    land_array_add(o->soup, t);
}
static void add_quad(Object * o, int a, int b, int c, int d) {
    int n = land_array_count(o->v);
    Float x = V(a) .x + V(b) .x + V(c) .x + V(d) .x;
    Float y = V(a) .y + V(b) .y + V(c) .y + V(d) .y;
    Float z = V(a) .z + V(b) .z + V(c) .z + V(d) .z;
    add_vertex(o, x / 4, y / 4, z / 4);
    add_triangle(o, a, b, n);
    add_triangle(o, b, c, n);
    add_triangle(o, c, d, n);
    add_triangle(o, d, a, n);
}
Object * object_new(void) {
    Object * o;
    land_alloc(o);
    o->v = land_array_new();
    o->soup = land_array_new();
    return o;
}
Object * object_sphere(int segments, int slices) {
    Object * o = object_new();
    for (int i = 0; i < slices; i += 1) {
        for (int j = 0; j < segments; j += 1) {
            Float angle = 2 * pi * j / segments;
            Float x = cos(angle);
            Float y = sin(angle);
            Float z = 2.0 * i / (slices - 1.0) - 1.0;
            Float r = sqrt(1 - z * z);
            x *= r;
            y *= r;
            add_vertex(o, x, y, z);
        }
    }
    for (int i = 0; i < slices - 1; i += 1) {
        for (int j = 0; j < segments; j += 1) {
            int row = i * segments;
            int row1 = i * segments + segments;
            add_quad(o, row + j, row1 + j, row1 + (j + 1) % segments, row + (j + 1) % segments);
        }
    }
    return o;
}
Object * object_cylinder_segments(int segments, bool top, bool bottom, int seg0, int seg1) {
    Object * o = object_new();
    int slices = 2;
    for (int i = 0; i < slices; i += 1) {
        for (int j = 0; j < segments; j += 1) {
            Float angle = 2 * pi * j / segments;
            Float x = cos(angle);
            Float y = sin(angle);
            Float z = 2.0 * i / (slices - 1.0) - 1.0;
            add_vertex(o, x, y, z);
        }
    }
    for (int i = 0; i < slices - 1; i += 1) {
        for (int j = 0; j < segments; j += 1) {
            int row = i * segments;
            int row1 = i * segments + segments;
            if (j < seg0 || j >= seg1) {
                continue;
            }
            add_quad(o, row + j, row1 + j, row1 + (j + 1) % segments, row + (j + 1) % segments);
        }
    }
    int bn = land_array_count(o->v);
    add_vertex(o, 0, 0, - 1);
    int tn = land_array_count(o->v);
    add_vertex(o, 0, 0, 1);
    if (bottom) {
        for (int j = 0; j < segments; j += 1) {
            if (j < seg0 || j >= seg1) {
                continue;
            }
            add_triangle(o, j, (j + 1) % segments, bn);
        }
    }
    if (top) {
        int row = (slices - 1) * segments;
        for (int j = 0; j < segments; j += 1) {
            if (j < seg0 || j >= seg1) {
                continue;
            }
            add_triangle(o, row + j, tn, row + (j + 1) % segments);
        }
    }
    if (seg0 != 0 || seg1 != segments) {
        add_quad(o, seg0, bn, tn, seg0 + segments);
        add_quad(o, bn, seg1, seg1 + segments, tn);
    }
    return o;
}
Object * object_cylinder(int segments, bool top, bool bottom) {
    return object_cylinder_segments(segments, top, bottom, 0, segments);
}
Object * object_cone(int segments, bool bottom) {
    Object * o = object_new();
    bool top = 1;
    for (int j = 0; j < segments; j += 1) {
        Float angle = 2 * pi * j / segments;
        Float x = cos(angle);
        Float y = sin(angle);
        Float z = - 1;
        add_vertex(o, x, y, z);
    }
    if (bottom) {
        int n = land_array_count(o->v);
        add_vertex(o, 0, 0, - 1);
        for (int j = 0; j < segments; j += 1) {
            add_triangle(o, j, (j + 1) % segments, n);
        }
    }
    if (top) {
        int n = land_array_count(o->v);
        add_vertex(o, 0, 0, 1);
        for (int j = 0; j < segments; j += 1) {
            add_triangle(o, j, n, (j + 1) % segments);
        }
    }
    return o;
}
Object * object_box(void) {
    Object * o = object_new();
    for (int z = 0; z < 2; z += 1) {
        for (int y = 0; y < 2; y += 1) {
            for (int x = 0; x < 2; x += 1) {
                add_vertex(o, x * 2 - 1, y * 2 - 1, z * 2 - 1);
            }
        }
    }
    add_quad(o, 0, 1, 3, 2);
    add_quad(o, 3, 1, 5, 7);
    add_quad(o, 2, 3, 7, 6);
    add_quad(o, 0, 2, 6, 4);
    add_quad(o, 0, 4, 5, 1);
    add_quad(o, 5, 4, 6, 7);
    return o;
}
Object * object_pyramid(void) {
    Object * o = object_new();
    for (int y = 0; y < 2; y += 1) {
        for (int x = 0; x < 2; x += 1) {
            add_vertex(o, x * 2 - 1, y * 2 - 1, - 1);
        }
    }
    add_vertex(o, 0, 0, 1);
    add_quad(o, 0, 1, 3, 2);
    add_triangle(o, 1, 0, 4);
    add_triangle(o, 3, 1, 4);
    add_triangle(o, 2, 3, 4);
    add_triangle(o, 0, 2, 4);
    return o;
}
Object * object_triangle(LandVector a, LandVector b, LandVector c) {
    Object * o = object_new();
    add_vertex(o, a.x, a.y, a.z);
    add_vertex(o, b.x, b.y, b.z);
    add_vertex(o, c.x, c.y, c.z);
    add_triangle(o, 0, 1, 2);
    return o;
}
void object_transform(Object * o, Land4x4Matrix * matrix) {
    {LandArrayIterator __iter1__ = LandArrayIterator_first(o->soup) ; for (Triangle * t = LandArrayIterator_item(o->soup, & __iter1__) ; LandArrayIterator_next(o->soup, & __iter1__) ; t = LandArrayIterator_item(o->soup, & __iter1__)) {
        t->a = land_vector_matmul(t->a, matrix);
        t->b = land_vector_matmul(t->b, matrix);
        t->c = land_vector_matmul(t->c, matrix);
    }
    }
}
void object_draw_lines(Object * o) {
    {LandArrayIterator __iter2__ = LandArrayIterator_first(o->soup) ; for (Triangle * t = LandArrayIterator_item(o->soup, & __iter2__) ; LandArrayIterator_next(o->soup, & __iter2__) ; t = LandArrayIterator_item(o->soup, & __iter2__)) {
        land_line(t->a.x, t->a.y, t->b.x, t->b.y);
        land_line(t->b.x, t->b.y, t->c.x, t->c.y);
        land_line(t->c.x, t->c.y, t->a.x, t->a.y);
    }
    }
}
