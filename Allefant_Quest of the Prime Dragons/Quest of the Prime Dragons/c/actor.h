#ifndef _ACTOR_
#define _ACTOR_
typedef struct Actor Actor;
typedef struct Tile Tile;
typedef struct Actors Actors;
#include "raytrace.h"
#include "graphics.h"
#include "actortype.h"
#include "main.h"
#include "item.h"
#include "ai.h"
extern int const tile_w;
extern int const tile_h;
struct Actor {
    int id;
    ActorType * kind;
    float x, y, angle;
    float tx, ty;
    int walk_tick;
    int attack_tick;
    int anim_tick;
    int special;
    int frozen;
    float hp;
    float max_hp;
    float damage;
    bool selected;
    bool dead;
    bool walking;
    Actor * target;
    bool projectile;
    float px, py;
};
struct Tile {
    LandArray * actors;
    Item * item;
};
struct Actors {
    LandArray * array;
    Actor * selected;
    int w, h;
    Tile * tilemap;
};
Actor * actor_new(Actors * actors, float x, float y, int type_id);
Actors * actors_new(void);
void actor_draw(Actor * self);
void actor_draw_shadow(Actor * self);
void actor_unplace(Actor * a);
bool actor_place(Actor * a);
void actor_hp(Actor * self, Actor * bywho, float amount);
void actor_avoid(Actor * self);
void actor_tick(Actor * self);
float actor_distance(Actor * self, Actor * other);
bool actor_attack(Actor * self);
void actor_projectile(Actor * self);
void actor_walk_within(Actor * self, Actor * other, float distance);
void actor_ranged_attack(Actor * self);
void actor_melee_attack(Actor * self);
void actor_target_action(Actor * self);
void actor_item(Actor * self, Item * it);
void actor_items(Actor * self);
void actors_draw(Actors * self);
void actors_unselect_all(Actors * self);
void actor_stop(Actor * a);
void actors_tick(Actors * self);
void actors_reset(Actors * self);
#endif
