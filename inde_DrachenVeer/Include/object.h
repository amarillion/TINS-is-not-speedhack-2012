#ifndef _OBJMANAGER_H_
#define _OBJMANAGER_H_

//Header file for managing objects and interactions via scripting.

#include <header.h>
#include <sprite.h>
#include <level.h>

// Might make this MAXSTR, but we don't need that many unique IDs for objects
#define MAX_NAME_LENGTH 32
#define MAX_OBJECTS 512

//Object types
#define OBJECT_UNKNOWN 0
#define OBJECT_PLAYER 1
#define OBJECT_ENEMY 2
#define OBJECT_ITEM 3
#define OBJECT_DOOR 4
#define OBJECT_PLATFORM 5
#define OBJECT_TRIGGER 6
#define OBJECT_NPC 7
#define OBJECT_ROPE 8
//Total number of types
#define OBJECT_TYPES 9

#define OBJECT_FACING_N 0
#define OBJECT_FACING_E 1
#define OBJECT_FACING_W 2
#define OBJECT_FACING_S 3
//Total number of facing directions
#define OBJECT_FACING_TYPES 4
#define OBJECT_FACING_DEFAULT 1

//Which form (composite shape) the object takes by default
#define DEFAULT_FORM SPRITESET_IDLE

//Object type names
extern const char object_typename_table[OBJECT_TYPES][MAXSTR];

//Forward declaration
struct _object;
typedef struct _object object;

//Object events
#define OBJECT_HANDLER_INIT 0
#define OBJECT_HANDLER_DESTROY 1
#define OBJECT_HANDLER_UPDATE 2
#define OBJECT_HANDLER_INTERACT 3
#define OBJECT_HANDLER_TYPES 4
const char* OBJECT_HANDLER_NAMES[OBJECT_HANDLER_TYPES];
typedef void* (*object_handler_func_ptr) (object*, void*);
typedef void* (*object_pair_handler_func_ptr) (object*, object *, void*);

typedef struct {
  object_handler_func_ptr _func[OBJECT_HANDLER_TYPES];
} object_handler;

//Forward declaration
struct _objectmanager;

//Main object structure
struct _object
{
//Unique numeric identifier set during creation
  unsigned int id;
  int pad;
//Angle in radians
  float angle;
//Point object model for Cartesian (X, Y) indexing
  float x, y, w, h;
//Weight: whether to apply gravity
  float wt;
//Health
  float health, health_max;
//Velocity: always retrieve from physics engine, unless manually controlled
  float vx, vy;
//Desired velocity: for control
  float vx_desired, vy_desired;
//Unique identifier for string indexing
  char name[MAX_NAME_LENGTH];
//Is this object allowed the third degree of freedom, i.e. rotate/spin?
  int canrotate;
//Type field
  int type;
//State
  int state;
//Which direction is it facing?
  int facing;
//Is it rogue? (non physics)
  int isrogue;
//Is this a static body? (if physics)
  int isstatic;
//Is a default bounding box to be used, or are custom shapes loaded?
  int customshapes;
//Object manager
  struct _objectmanager *omanager;
//Can this object be re-used?
  int garbage, setlocation;
//Handlers
  int pad3;
//Layer
  int layer;
  object_handler handlers;
//Attached sprite
  spriteinstance *sprinstance;
//Attached physics
  void *physicsdata;
};
typedef object** objectptr_iterator;

//Additional data needed for object creation alone
typedef struct _ropedata
{
  float w, h;
  int nsegments;
} ropedata;

//Set of all objects in the game/level
typedef struct _objectmanager
{
  object *objectset[MAX_OBJECTS];
  objectptr_iterator begin, end;
  level *level;			//For easy access.
  int pad;
} objectmanager;

//Allocate a new object manager
objectmanager *om_start (level * l);

//Deallocate an object manager
void om_end (objectmanager * om);

//Number of objects
int om_get_number_of_objects(objectmanager * om);

//Center camera on an object
void om_center_camera_on (object * ptr);

//Retrieve object by name
object *om_get_object_by_name (const char *name);

//Create a new object
object *om_create (objectmanager * om);

//Update its state and add it to physics
void om_add (objectmanager * om, object * newobject, void * data);

//Mark the object as garbage, so it can be re-used
void om_dispose (objectmanager * om, object * object);

//Instantiate a sprite, to attach to an object
spriteinstance *om_create_sprite_instance (objectmanager * om,
					   const char *spritename);

//Currently active objectmanager instance
//NOTE: curlevel cannot be used during script execution because it is updated only post-loading.
//      So we are using pointers to work around this.
//Not correct: objectmanager *om_get_active_objectmanager();
extern objectmanager *get_scriptload_objectmanager ();

//For debugging
void om_dump_object (object * ptr);

//Add custom callback
void om_set_handler (object * ptr, int handler_type, void *(*handler) (object * obj, void *data));

//Updates the states of all objects
int om_update (void *omgr, float dt);

//Load ObjectMap from a pre opened file
void *load_objmap_file (FILE * fobj, char *levelname, objectmanager * om);

//Default handlers
void landed_player (object * hero, void *data);

//Render Object.
void render_object (ALLEGRO_BITMAP * bitmap, object *obj, int stx, int sty, int flags);

//Interact between Two Objects.
void om_interact(object *a, object *b, void *data);

//Set Interaction Callback between Two Objects.
void om_set_interaction(object *a, object *b, object_pair_handler_func_ptr func);

//Retrieve/Set object state either based on spriteinstance,
// or internal state if sprinstance does not exist.
int om_get_object_state(object *obj);
void om_set_object_state(object *obj, int state);

//Ends an object's lifetime within objectmanager by:
//  + removing it's physics (optional)
//  + marking it as garbage
void om_remove_object(objectmanager *om, object *obj);

#endif //_OBJMANAGER_H_
