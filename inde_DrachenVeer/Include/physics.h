#ifndef _PHYSICS_H_
#define _PHYSICS_H_

#include <level.h>

#define PHYSICS_PARAM_SIMERR 0
#define PHYSICS_PARAM_CFM 1
#define PHYSICS_PARAM_GRAVITY 2

#define DIR_NONE (0)
#define DIR_LEFT (1)
#define DIR_RIGHT (1<<1)
#define DIR_UP (1<<2)
#define DIR_DOWN (1<<3)

#define SIM_DT (1.0f/120.0f)

#define SHAPE_CIRC 0
#define SHAPE_POLY 1
#define SHAPE_NUM 2

#define VERTICES_MAX 512

typedef struct
{
  int body_a, body_b;
  float x[2], y[2];
  float min, max;
} constraint_data;

//Initialize Physics Engine.
int physics_init ();

//Quit Physics Engine.
void physics_quit ();

//Start Physics Engine and returns Physics Context.
void *physics_start (void *params);

//End Physics Engine.
void physics_end (void *physics);

//Set Physics Parameters
void physics_setparam (void *physics, int paramid, void *value);

//Create a Physics Object
void *physics_create_object (void *physics, void *omdata);

//Attach a body to the object for collision dynamics
void physics_add_bodies(void *physics, void *om_obj);

//Attach shapes to the object for collision callbacks
void physics_add_shapes (void *physics, void *omobj);

//Update Physics Frames
void physics_update (void *physics, float dt);

//Get Position, Rotation of Object
void physics_getpose (void *physics, void *data, float *vector3);

//Get Velocity
void physics_getvel (void *physics, void *data, float *vector3);

//Load Attributes
void physics_load_attributemap (void *physics, void *data);

//Custom collision handler for special "water" tiles
void physics_addwater (void *physics, float x1, float y1, float x2, float y2);

//Perform Jump
void physics_setjump (void *physics, void *data, int jump);

//Set Physics Direction
void physics_setdirection (void *physics, void *data, int dir);

//Get Physics Direction
int physics_getdirection (void *physics, void *data);

//Shape visitor, e.g. for debugging
void physics_visit_shapes(void *physics, void (*shape_visitor)(void *, void *), void *data);

//Notify Physics Engine to Update Shape
void physics_shape_changed(void *physics, void *objptr);

//Render Physics
void render_physics (ALLEGRO_BITMAP *bitmap, void *physics, int stx, int sty);

//Notify physics engine to change the form of an object
void physics_morph_form(void *physics, void *cpo, int to);

//Deallocate bodies and shapes
void physics_cleanup_object(void *physics, void *cpobjptr);

void physics_create_body_list (void *phy, void *omdata, int nbodies);
void physics_set_body_list (void *physics, void *omdata, int which, int isstatic, float bx, float by, float brot, float mass, float sizew, float sizeh);
void physics_create_constraint_list (void *phy, void *omdata, int nconstraints);
void physics_set_constraint_list (void *physics, void *omdata, int which, char *type, void *cdata);
void physics_create_shape_list (void *phy, void *omdata, int nshapes);
void physics_set_shape_list (void *phy, void *omdata, int nvert, float *xvert, float *yvert, int which, int b);

//Add custom shape to object
void physics_attach_shape_to_body (void *physics, void *omdata, int shape, int body);

void physics_set_constraint_list_global (void *physics, int which, char *type, void *omdataA, void *omdataB, void *cdata);
void physics_add_constraints_to_space (void *physics);

void physics_dump_info (void *physics, void *omdata);

void physics_add_rope (void *physics, void *omdata);

#endif //_PHYSICS_H_
