#ifndef _SPRITE_H_
#define _SPRITE_H_

//Header file containing sprite structures, function declarations and sprite specific definitions.

#include <header.h>

//Types of Sprite Set
#define SPRITESET_NULL   -1
#define SPRITESET_WALK    0
#define SPRITESET_RUN     1
#define SPRITESET_SLOW    2
#define SPRITESET_STOP    3
#define SPRITESET_JUMP    4
#define SPRITESET_FLY     5
#define SPRITESET_CROUCH  6
#define SPRITESET_ATTACK  7
#define SPRITESET_HIT     8
#define SPRITESET_DEAD    9
#define SPRITESET_IDLE    10
#define SPRITESET_CLIMB   11
#define SPRITESET_DYING   12

//Total Number of Types
#define SPRITESET_TYPES   13

//Types of Sprite Draw
#define SPRITEDRAW_NULL  -1
#define SPRITEDRAW_FULL   0
#define SPRITEDRAW_INCR   1
#define SPRITEDRAW_COLOR  2

//Types of Sprite Collision
#define SPRITECOLL_NULL   -1
#define SPRITECOLL_RECT    0
#define SPRITECOLL_PIXEL   1
#define SPRITECOLL_CIRC    2

//Integrate with other possible render flags
#define RENDER_SPRITE_FLIP_HORZ 1
#define RENDER_SPRITE_FLIP_VERT (1 << 1)

//Sprite Set Frame for Drawing
struct _spritesetframe
{
  int value, extra_value;
  int draw_mode, coll_mode;
};

typedef struct _spritesetframe spritesetframe;

//Sprite Set
struct _spriteset
{
  int nframes;
  int type;
  spritesetframe *frames;
  int pad;
};

typedef struct _spriteset spriteset;

//Sprite
struct _sprite
{
  int spritewidth, spriteheight;
  ALLEGRO_BITMAP *spritebitmap;
  spriteset *spritesets[SPRITESET_TYPES];
};

typedef struct _sprite sprite;

//Sprite instance to share sprites
struct _spriteinstance
{
  int cursst;
  int curframe;
  int prevsst;
  int pad;
  sprite *sprite;
};

typedef struct _spriteinstance spriteinstance;

//Unload Sprite
void unload_sprite (sprite * givsprite);

//Load Sprite from a file
sprite *load_sprite (char *spritename);

//Uninitialize Sprite Animater
void quit_sprite_animater ();

//Initialize Sprite Animater
int init_sprite_animater ();

//Render sprite.
void render_sprite (ALLEGRO_BITMAP * bitmap, spriteinstance * givsi, int xcursor, int ycursor, float angle, int sprite_flags);

//Animate Sprite
int animate_sprite (spriteinstance * givsprite, int flags);

//Change Sprite State
void sprite_set_state (spriteinstance * givsi, int newsst);

//Retrieve Sprite State
int sprite_get_state (spriteinstance * givsi);

//Spriteset Type to String
const char * spriteset_type_to_str(int sst);

#endif //_SPRITE_H_
