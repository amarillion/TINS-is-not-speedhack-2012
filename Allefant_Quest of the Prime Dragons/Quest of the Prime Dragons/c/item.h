#ifndef _ITEM_
#define _ITEM_
typedef struct Item Item;
#include "itemtype.h"
#include "actor.h"
struct Item {
    int kind;
    float x, y;
    float angle;
};
Item * item_new(int kind, float x, float y, float angle);
void item_place(Item * i);
void item_remove(Item * i);
void item_draw(Item * self);
void item_draw_shadow(Item * self);
#endif
