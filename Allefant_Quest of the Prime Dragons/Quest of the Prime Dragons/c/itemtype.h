#ifndef _ITEMTYPE_
#define _ITEMTYPE_
typedef struct ItemType ItemType;
#include "raytrace.h"
#include "graphics.h"
struct ItemType {
    int id;
    int w, h;
    char const * graphics;
    LandImage * images[8];
    LandImage * shadows[8];
    Raytracer * traces[8];
    Raytracer * shadow_traces[8];
};
enum {
    IT_NONE,
    IT_MOUNTAIN,
    IT_CLOUD,
    IT_CLOUD2,
    IT_WALL,
    IT_WALL2,
    IT_CHEESE,
    IT_ARROW,
    IT_MAGIC_MISSILE,
    IT_HEAL,
    IT_FIREBALL,
    IT_COUNT
};
extern ItemType * item_types[IT_COUNT];
void item_type_render(ItemType * self);
ItemType * item_type_new(char const * script, int w, int h, int id);
void items_init(void);
#endif
