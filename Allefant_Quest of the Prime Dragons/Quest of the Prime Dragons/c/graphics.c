#include "graphics.h"
typedef struct ParserState ParserState;
static void trace(Raytracer * r) {
    unsigned char * rgba = raytracer_trace(r);
    unsigned char * p2 = r->rgba;
    for (int y = 0; y < r->h >> 2; y += 1) {
        for (int x = 0; x < r->w >> 2; x += 1) {
            int cr = 0, cg = 0, cb = 0, ca = 0;
            for (int v = 0; v < 4; v += 1) {
                for (int u = 0; u < 4; u += 1) {
                    unsigned char * p = rgba + (y * 4 + v) * r->w * 4 + (x * 4 + u) * 4;
                    cr += p[0];
                    cg += p[1];
                    cb += p[2];
                    ca += p[3];
                }
            }
            p2[0] = cr >> 4;
            p2[1] = cg >> 4;
            p2[2] = cb >> 4;
            p2[3] = ca >> 4;
            p2 += 4;
        }
    }
    land_free(rgba);
}
struct ParserState {
    float value;
    bool negative;
    int place;
    Land4x4Matrix m;
    int st;
    Land4x4Matrix stack[16];
    int ji;
    int jump[16];
    int loop[16];
    float red, green, blue, alpha;
};
static void reset(ParserState * s) {
    s->value = 0;
    s->negative = 0;
    s->place = 0;
}
static void finish(ParserState * s) {
    if (s->place == 0) {
        s->value = 1;
        s->place = 1;
    }
    if (s->negative) {
        s->value = - s->value;
        s->negative = 0;
    }
}
Raytracer * graphics_parse(char const * script, int w, int h, Land4x4Matrix matrix, double dt) {
    /* S sphere
     * C cone
     * B box
     * R rod
     * P pyramid
     * H half cylinder
     * x/y/z translate
     * s/sx/sy/sz scale
     * rx/ry/rz rotate
     * () preserve transformation
     * t multiply number with time
     * <number> value for following command
     * <name> color
     */
    Raytracer * r = raytracer_new(w, h);
    LandVector light = land_vector(0, 0, 1);
    light = land_vector_rotate(light, land_vector(1, 0, 0) , pi / 8);
    light = land_vector_rotate(light, land_vector(0, 1, 0) , pi / 4);
    raytracer_add_light(r, light);
    char c;
    ParserState s;
    memset( & s, 0, sizeof s);
    s.m = matrix;
    s.alpha = 1;
    for (int i = 0; (c = script[i]) ; i++) {
        char n = script[i + 1];
        if (c == '#') {
            while (script[i + 1] && script[i + 1] != '\n') {
                i++;
            }
        }
        else if(c == '-') {
            s.negative = 1;
        }
        else if(c == '.') {
            s.place = - 1;
        }
        else if(c == 't') {
            finish( & s);
            s.value *= dt;
        }
        else if(c >= '0' && c <= '9') {
            float v = c - '0';
            if (s.place < 0) {
                s.value += pow(10, s.place) * v;
                s.place--;
            }
            else {
                s.value *= 10;
                s.value += v;
                s.place++;
            }
        }
        else if(c == '(') {
            s.stack[s.st++] = s.m;
        }
        else if(c == ')') {
            s.m = s.stack[--s.st];
        }
        else if(c == '{') {
            finish( & s);
            s.jump[s.ji] = i;
            s.loop[s.ji] = s.value;
            s.ji++;
            reset( & s);
        }
        else if(c == '}') {
            s.loop[s.ji - 1] --;
            if (s.loop[s.ji - 1] > 0) {
                i = s.jump[s.ji - 1];
            }
            else {
                s.ji--;
            }
        }
        else if(c == 'x' || c == 'y' || c == 'z') {
            finish( & s);
            float x = c == 'x' ? s.value : 0;
            float y = c == 'y' ? s.value : 0;
            float z = c == 'z' ? s.value : 0;
            Land4x4Matrix m = land_4x4_matrix_translate(x, y, z);
            s.m = land_4x4_matrix_mul(s.m, m);
            reset( & s);
        }
        else if(c == 'r' &&(n == 'x' || n == 'y' || n == 'z')) {
            finish( & s);
            LandVector a = land_vector(1, 0, 0);
            if (n == 'y') {
                a = land_vector(0, 1, 0);
            }
            if (n == 'z') {
                a = land_vector(0, 0, 1);
            }
            Land4x4Matrix m = land_4x4_matrix_rotate(a.x, a.y, a.z, pi * s.value);
            s.m = land_4x4_matrix_mul(s.m, m);
            reset( & s);
            i++;
        }
        else if(c == 's') {
            finish( & s);
            float x = s.value;
            float y = s.value;
            float z = s.value;
            if (n == 'x') {
                i++;
                y = z = 1;
            }
            if (n == 'y') {
                i++;
                x = z = 1;
            }
            if (n == 'z') {
                i++;
                y = x = 1;
            }
            Land4x4Matrix m = land_4x4_matrix_scale(x, y, z);
            s.m = land_4x4_matrix_mul(s.m, m);
            reset( & s);
        }
        else if(c == 'S' || c == 'R' || c == 'C' || c == 'B' || c == 'P' || c == 'H' || c == 'T') {
            finish( & s);
            Object * o = NULL;
            if (c == 'S') {
                o = object_sphere(16, 16);
            }
            if (c == 'R') {
                o = object_cylinder(16, 1, 1);
            }
            if (c == 'C') {
                o = object_cone(16, 1);
            }
            if (c == 'B') {
                o = object_box();
            }
            if (c == 'P') {
                o = object_pyramid();
            }
            if (c == 'H') {
                o = object_cylinder_segments(16, 1, 1, 0, 8);
            }
            if (c == 'T') {
                o = object_cylinder_segments(16, 1, 1, 0, 12);
            }
            object_rgba(o, s.red, s.green, s.blue, s.alpha);
            Land4x4Matrix t = land_4x4_matrix_mul(s.m, land_4x4_matrix_scale(s.value, s.value, s.value));
            object_transform(o, & t);
            raytracer_add_object(r, o);
            reset( & s);
        }
        else if(c == '/') {
            for (int ic = 0; colors[ic] .name; ic++) {
                if (! strncmp(colors[ic] .name, script + i + 1, strlen(colors[ic] .name))) {
                    i += strlen(colors[ic] .name);
                    s.red = colors[ic] .r / 255.0;
                    s.green = colors[ic] .g / 255.0;
                    s.blue = colors[ic] .b / 255.0;
                    break;
                }
            }
        }
    }
    trace(r);
    return r;
}
