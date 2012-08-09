#ifndef _ACTORTYPE_
#define _ACTORTYPE_
typedef struct ActorType ActorType;
#include "raytrace.h"
#include "graphics.h"
#include "itemtype.h"
enum {
    AT_HUNTER,
    AT_DOG,
    AT_HEALER,
    AT_DRAKE,
    AT_DRAGON,
    AT_GROLL,
    AT_PAPAGROLL,
    AT_KNIGHT,
    AT_MAGE,
    AT_BAT,
    AT_WOLF,
    AT_COUNT
};
extern char const * actor_names[AT_COUNT];
struct ActorType {
    int w, h;
    int level;
    int initial_hp;
    int initial_damage;
    char const * graphics;
    LandImage * images[40];
    LandImage * shadows[40];
    Raytracer * traces[40];
    Raytracer * shadow_traces[40];
    bool enemy;
    int projectile;
    float projectile_speed;
    int id;
};
extern ActorType * actor_type[AT_COUNT];
void actor_render(ActorType * self);
ActorType * actor_type_new(int w, int h, int kind);
ActorType * actor_hunter_new(void);
ActorType * actor_knight_new(void);
ActorType * actor_healer_new(void);
ActorType * actor_mage_new(void);
ActorType * actor_drake_new(void);
ActorType * actor_dragon_new(void);
ActorType * actor_dog_new(void);
ActorType * actor_groll_new(void);
ActorType * actor_papagroll_new(void);
ActorType * actor_bat_new(void);
ActorType * actor_wolf_new(void);
void actor_types_init(void);
#endif
