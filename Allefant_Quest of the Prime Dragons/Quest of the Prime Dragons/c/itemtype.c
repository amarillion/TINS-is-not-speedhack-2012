#include "itemtype.h"
ItemType * item_types[IT_COUNT];
static ItemType * global_current;
static int thread_data[8];
static LandThread * thread[8];
static void cb(void * data) {
    ItemType * self = global_current;
    int * ip = data;
    int i = * ip;
    Land4x4Matrix t = land_4x4_matrix_identity();
    t = land_4x4_matrix_mul(land_4x4_matrix_rotate(1, 0, 0, pi * - 0.333) , t);
    t = land_4x4_matrix_mul(t, land_4x4_matrix_rotate(0, 0, 1, 2 * pi * (i & 7) / 8));
    t = land_4x4_matrix_mul(t, land_4x4_matrix_scale(32, 32, 32));
    self->traces[i] = graphics_parse(self->graphics, self->w * 4, self->h * 4, t, (i >> 3) / 2.0 - 1.0);
    t = land_4x4_matrix_identity();
    t = land_4x4_matrix_mul(t, land_4x4_matrix_rotate(0, 0, 1, 2 * pi * (i & 7) / 8));
    t = land_4x4_matrix_mul(t, land_4x4_matrix_scale(32, 32, 32));
    self->shadow_traces[i] = graphics_parse(self->graphics, self->w * 4, self->h * 4, t, (i >> 3) / 2.0 - 1.0);
}
void item_type_render(ItemType * self) {
    global_current = self;
    for (int i = 0; i < 8; i += 1) {
        thread_data[i] = i;
        thread[i] = land_thread_new(cb, thread_data + i);
    }
    for (int i = 0; i < 8; i += 1) {
        land_thread_destroy(thread[i]);
    }
    for (int i = 0; i < 8; i += 1) {
        self->images[i] = land_image_new(self->w, self->h);
        land_image_set_rgba_data(self->images[i] , self->traces[i] ->rgba);
        unsigned char * p = self->shadow_traces[i] ->rgba;
        for (int y = 0; y < self->h; y += 1) {
            for (int x = 0; x < self->w; x += 1) {
                p[0] = p[1] = p[2] = 0;
                p[3] /= 4;
                p += 4;
            }
        }
        self->shadows[i] = land_image_new(self->w, self->h);
        land_image_set_rgba_data(self->shadows[i] , self->shadow_traces[i] ->rgba);
    }
}
ItemType * item_type_new(char const * script, int w, int h, int id) {
    ItemType * self;
    land_alloc(self);
    self->id = id;
    item_types[id] = self;
    self->w = w;
    self->h = h;
    self->graphics = script;
    item_type_render(self);
    return self;
}
void items_init(void) {
    item_type_new("\n"
        "/olive\n"
        "ry\n"
        "2.5P\n"
        "-z\n"
        "1.5x.2rz1.5P\n"
        "", 64, 64, IT_MOUNTAIN);
    item_type_new("\n"
        "/lightcyan\n"
        "2S\n"
        "zy2x1.5S\n"
        "1x-1y1S\n"
        "", 64, 64, IT_CLOUD);
    item_type_new("\n"
        "/lightblue\n"
        ".6s\n"
        "2S\n"
        "zy2x1.5S\n"
        "1x-1y1S\n"
        "", 64, 64, IT_CLOUD2);
    item_type_new("\n"
        "/peru\n"
        ".1rz\n"
        "1B\n"
        "", 32, 32, IT_WALL);
    item_type_new("\n"
        "/brown\n"
        ".1rz\n"
        "3szB\n"
        "", 32, 64, IT_WALL2);
    item_type_new("\n"
        "/gold\n"
        ".5sz1.1rzT\n"
        "", 16, 16, IT_CHEESE);
    item_type_new("\n"
        "/moccasin\n"
        ".5rx10sz.1R\n"
        "", 16, 16, IT_ARROW);
    item_type_new("\n"
        "/hotpink\n"
        ".5rx5szry.2C\n"
        "", 16, 16, IT_MAGIC_MISSILE);
    item_type_new("\n"
        "/white\n"
        ".5S\n"
        "", 16, 16, IT_HEAL);
    item_type_new("\n"
        "/orange\n"
        "S\n"
        "", 16, 16, IT_FIREBALL);
}
