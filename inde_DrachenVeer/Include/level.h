#ifndef _LEVEL_H_
#define _LEVEL_H_

//Header file containing level structures, function declarations and level specific definitions.

#include <header.h>
#include <sprite.h>
#include <physics.h>

//Types of Level Layers
#define LEVEL_NULL     -1
#define LEVEL_COLORMAP  1
#define LEVEL_PARALLAX  2
#define LEVEL_TILEMAP   3
#define LEVEL_ATTRMAP   4
#define LEVEL_SPRITEMAP 5
#define LEVEL_SCRIPT    6
#define LEVEL_OBJMAP    7

//Default Values
#define ATTR_NULL -1
#define TILE_NULL -1
#define TILESET_NULL -1

//Types of level renderers (for debugging)
#define LEVRENDER_NULL 0
#define LEVRENDER_NORMAL 1
#define LEVRENDER_DEBUG_OBJECTS 2
#define LEVRENDER_DEBUG_PHYSICS 4
#define LEVRENDER_HEALTHBARS 8

//Maximum number of sprite instances
#define SPRITE_INSTANCES 512

//Tile Set Frame for Drawing
struct _tilesetframe
{
  int filler;
  int top, bottom, left, right;
  int tl, tr, bl, br;
  int pad[3];
};

typedef struct _tilesetframe tilesetframe;

//Tile Set
struct _tileset
{
  int nframes;
  int curframe;
  int pad[2];
  tilesetframe *frames;
};

typedef struct _tileset tileset;

//Tile Map
struct _tilemap
{
  int width, height;
  int tilewidth, tileheight;
  int ntilesets;
  int pad[3];
  int *data;
  tileset *tilesets;
  ALLEGRO_BITMAP *tilebitmap;
  void *dummy;
};

typedef struct _tilemap tilemap;

//Attribute Map
struct _attrmap
{
  int width, height;
  int tilewidth, tileheight;
  int *data;
  int pad[3];
};

typedef struct _attrmap attrmap;

//Parallax Layer Map
struct _parallaxmap
{
  ALLEGRO_BITMAP *pxbitmap;
  int speed;
  int alpha;
  int pad;
};

typedef struct _parallaxmap parallaxmap;

//Color Layer Map
struct _colormap
{
  int r, g, b, a;
};

typedef struct _colormap colormap;

//Sprite Map
struct _spritemap
{
  int nsprites;
  char **spritenames;
  sprite **sprites;
  int nspriteinstances;
  spriteinstance spriteinstances[SPRITE_INSTANCES];
};

typedef struct _spritemap spritemap;

//Level Layer Map
struct _level
{
  int nlayers;
  int *layertypes;
  int **layers;			//Void Pointers.
  int score;			//Player score.
  attrmap *am;			//Easy Access.
  void *cam;                    //Camera.
  spritemap *sprm;		//Easy Access.
  void *objectmanager;		//Stores all objects.
  void *physics;		//Physics, If Needed.
  void *dummy[3];		//Unused
};

typedef struct _level level;

//Unload Level
void unload_level (level * givlevel);

//Load Level from a file
level *load_level (int levelnum);

//Unload all Levels
void unload_levels ();

//Load all Levels
int load_levels ();

//Uninitialize Level Animater
void quit_level_animater ();

//Initialize Level Animater
int init_level_animater ();

//Animate Level
int animate_level (level * givlevel, int camx, int camy, int flags);

//Uninitialize Level Renderer
void quit_level_renderer ();

//Initialize Level Renderer
int init_level_renderer ();

//Render Level
void render_level (ALLEGRO_BITMAP * bitmap, level * givlevel, int camx,
		   int camy, int flags);

void
transform_coords (ALLEGRO_BITMAP *bitmap, level *givlev,
		int camx, int camy, int xspeed, int *stxptr, int *styptr);

#endif //_LEVEL_H_
