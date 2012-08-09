#include "raytrace.h"
static Float ray_intersects_triangle_optimized(Ray ray, Triangle tri) {
    /* If the ray originating at pos, running into dir, hits the triangle defined
     * by its vertices v0, v1, v2, return 1 and store the point of collision in
     * result. Otherwise return 0 and store nothing.
     * Triangles are one-sided.
     */
    Vector eu = land_vector_sub(tri.b, tri.a);
    Vector ev = land_vector_sub(tri.c, tri.a);
    float det = eu.x * ev.y - eu.y * ev.x;
    if (det <= 0) {
        return 0;
    }
    Vector vt = land_vector_sub(ray.a, tri.a);
    Float u = vt.x * ev.y - vt.y * ev.x;
    if (u < 0 || u > det) {
        return 0;
    }
    Vector cu = land_vector_cross(vt, eu);
    Float v = - cu.z;
    if (v < 0 || u + v > det) {
        return 0;
    }
    Float t = land_vector_dot(ev, cu);
    if (t < 0) {
        return 0;
    }
    t /= det;
    return t;
}
Raytracer * raytracer_new(int w, int h) {
    Raytracer * self;
    land_alloc(self);
    self->objects = land_array_new();
    self->w = w;
    self->h = h;
    self->tiles = land_calloc(w / 8 * h / 8 * sizeof * self->tiles);
    self->rgba = land_malloc(w / 4 * h / 4 * 4 * sizeof * self->rgba);
    return self;
}
void raytracer_add_object(Raytracer * self, Object * o) {
    land_array_add(self->objects, o);
}
void raytracer_add_light(Raytracer * self, Vector v) {
    self->light = v;
}
void minmax3(float a, float b, float c, float * vmin, float * vmax) {
    if (a < b) {
        if (a < c) {
            * vmin = a;
            if (b < c) {
                * vmax = c;
            }
            else {
                * vmax = b;
            }
        }
        else {
            * vmin = c;
            * vmax = b;
        }
    }
    else {
        if (a < c) {
            * vmin = b;
            * vmax = c;
        }
        else {
            * vmax = a;
            if (b < c) {
                * vmin = b;
            }
            else {
                * vmin = c;
            }
        }
    }
}
unsigned char * raytracer_trace(Raytracer * self) {
    float * zbuffer = land_calloc(self->w * self->h * sizeof * zbuffer);
    unsigned char * rgba = land_calloc(self->w * self->h * 4);
    {LandArrayIterator __iter0__ = LandArrayIterator_first(self->objects) ; for (Object * o = LandArrayIterator_item(self->objects, & __iter0__) ; LandArrayIterator_next(self->objects, & __iter0__) ; o = LandArrayIterator_item(self->objects, & __iter0__)) {
        object_recalc_normals(o);
        {LandArrayIterator __iter1__ = LandArrayIterator_first(o->soup) ; for (Triangle * tri = LandArrayIterator_item(o->soup, & __iter1__) ; LandArrayIterator_next(o->soup, & __iter1__) ; tri = LandArrayIterator_item(o->soup, & __iter1__)) {
            float minx, miny, maxx, maxy;
            minmax3(tri->a.x, tri->b.x, tri->c.x, & minx, & maxx);
            minmax3(tri->a.y, tri->b.y, tri->c.y, & miny, & maxy);
            int tx1 = (int) (self->w / 2 + minx) >> 3;
            int ty1 = (int) (self->h / 2 + miny) >> 3;
            int tx2 = (int) (self->w / 2 + maxx) >> 3;
            int ty2 = (int) (self->h / 2 + maxy) >> 3;
            if (tx1 < 0) {
                tx1 = 0;
            }
            if (ty1 < 0) {
                ty1 = 0;
            }
            if (tx2 > (self->w >> 3) - 1) {
                tx2 = (self->w >> 3) - 1;
            }
            if (ty2 > (self->h >> 3) - 1) {
                ty2 = (self->h >> 3) - 1;
            }
            for (int v = ty1; v < ty2 + 1; v += 1) {
                for (int u = tx1; u < tx2 + 1; u += 1) {
                    if (! self->tiles[v * (self->w >> 3) + u]) {
                        self->tiles[v * (self->w >> 3) + u] = land_array_new();
                    }
                    land_array_add(self->tiles[v * (self->w >> 3) + u] , tri);
                }
            }
        }
        }
    }
    }
    for (int v = 0; v < self->h >> 3; v += 1) {
        for (int u = 0; u < self->w >> 3; u += 1) {
            LandArray * triangles = self->tiles[v * (self->w >> 3) + u];
            if (! triangles) {
                continue;
            }
            Ray ray;
            ray.u = land_vector(0, 0, - 1);
            unsigned char * p = rgba + u * 8 * 4 + v * 8 * self->w * 4;
            float * zp = zbuffer + u * 8 + v * 8 * self->w;
            for (int y = - self->h / 2 + v * 8; y < - self->h / 2 + v * 8 + 8; y += 1) {
                for (int x = - self->w / 2 + u * 8; x < - self->w / 2 + u * 8 + 8; x += 1) {
                    ray.a = land_vector(x + 0.5, y + 0.5, 1024);
                    {LandArrayIterator __iter2__ = LandArrayIterator_first(triangles) ; for (Triangle * tri = LandArrayIterator_item(triangles, & __iter2__) ; LandArrayIterator_next(triangles, & __iter2__) ; tri = LandArrayIterator_item(triangles, & __iter2__)) {
                        Float t = ray_intersects_triangle_optimized(ray, * tri);
                        if (t > * zp) {
                            Float l = land_vector_dot(tri->n, self->light);
                            float d1 = (1.0 + l) / 2.0;
                            float d2 = (1.0 - l) / 8.0;
                            int c = (d1 + d2) * 255;
                            if (c < 0) {
                                c = 0;
                            }
                            if (c > 255) {
                                c = 255;
                            }
                            p[0] = tri->o->r * c;
                            p[1] = tri->o->g * c;
                            p[2] = tri->o->b * c;
                            p[3] = 255;
                            * zp = t;
                        }
                    }
                    }
                    p += 4;
                    zp++;
                }
                p += (self->w - 8) * 4;
                zp += self->w - 8;
            }
        }
    }
    land_free(zbuffer);
    return rgba;
}
void raytracer_draw_lines(Raytracer * r) {
    {LandArrayIterator __iter3__ = LandArrayIterator_first(r->objects) ; for (Object * o = LandArrayIterator_item(r->objects, & __iter3__) ; LandArrayIterator_next(r->objects, & __iter3__) ; o = LandArrayIterator_item(r->objects, & __iter3__)) {
        object_draw_lines(o);
    }
    }
}
