#include <header.h>
#include <log.h>
#include <math.h>
#include <physics.h>
#include "Private/physicschipmunk.h"
#include <chipmunk/chipmunk.h>
#include <chipmunk/constraints/util.h>
#include <object.h>

//Declarations
#define MAX_CHIPMUNK_OBJECTS 256
#define MAX_CHIPMUNK_CONSTRAINTS 256
#define CHIPMUNK_SLEEP_TIME (0.5f)
#define DEFAULT_MASS (0.3f)
#define DEFAULT_WIDTH (1.0f)
#define DEFAULT_HEIGHT (1.0f)
#define DEFAULT_FRICTION (0.7f)
#define DEFAULT_ELASTICITY (0.1f)
#define DEFAULT_DAMPING (1.0f)
#define FLUID_DENSITY (1.0f)
#define FLUID_DRAG (2.0f)	//0.00014f
#define THICK 2.0f

//Character Movement based on Chipmunk Platform demo
#define GRAVITY_EARTH 400.0f
//used to be 200.0f

#define NV 8
#define CHAIN_COUNT 4
#define LINK_COUNT 3
#define COLLISION_SLOP 0.1f
#define PLAYER_VELOCITY 100.0f
#define PLAYER_GROUND_ACCEL_TIME 0.1f
#define PLAYER_GROUND_ACCEL (PLAYER_VELOCITY/PLAYER_GROUND_ACCEL_TIME)
#define PLAYER_AIR_ACCEL_TIME 0.1f
#define PLAYER_AIR_ACCEL (PLAYER_VELOCITY/PLAYER_AIR_ACCEL_TIME)
#define JUMP_HEIGHT 60.0f
#define JUMP_BOOST_HEIGHT 20.0f
#define FALL_VELOCITY 200.0f
#define JUMP_LENIENCY 0.05f

//Forward declaration
typedef cpShape shape;
DECLARE_LIST(shape);
typedef cpBody body;
DECLARE_LIST(body);
typedef cpConstraint constraint;
DECLARE_LIST(constraint);
struct _grounding_context;
typedef struct _grounding_context grounding_context;
struct _levelphysics;
typedef struct _levelphysics levelphysics;

//Internal API
static cpBool waterPreSolve (cpArbiter * arb, cpSpace * space, void *ptr);
static void physics_cleanup_body (void *physics, cpBody * objptr, void *data);
static void
physics_cleanup_shape (void *physics, cpShape * sobjptr, void *data);
static void
add_box (levelphysics * chipmunk, cpFloat x1, cpFloat y1, cpFloat x2,
	 cpFloat y2);
static void callback_grounding_context (cpBody * body, cpArbiter * arb,
					grounding_context * grounding);
static void
update_grounding_context (cpBody * body, grounding_context * context);
//static void shape_list_create (shape_list ** slist, int num_shapes);
//static void shape_list_cleanup (shape_list * slist);
//static void shape_list_set (shape_list * slist, int which, shape * s);
static void render_polygon (void *shape, void *data);
//static void postStepMorphShape(cpSpace *space, void *obj, void *data);
void physics_reset_object(void *cpobj);

//Types
struct _grounding_context
{
  cpVect normal;
  cpVect impulse;
  cpFloat penetration;
  cpBody *body;
};

typedef struct _chipmunkobject
{
  body_list *bodies;
  constraint_list *constraints;
  //shape_list *forms[SPRITESET_TYPES];
  shape_list *forms;
  int groundingShape;
  cpFloat mass;
  cpFloat moment;
  grounding_context grounding;
  cpVect groundVelocity;
  cpFloat remainingBoost, remainingJumpLeniency;
  int remainingAirJumps;
  int directionInput, jumpInput;
  int wellGrounded;
  int lastJumpState;
  int canDoubleJump;
  void *omdata;
} chipmunkobject;

struct _levelphysics
{
  cpSpace *space;
  int _num_objects;
  float _elapsed;
  chipmunkobject _objects[MAX_CHIPMUNK_OBJECTS];
  constraint_list *constraints;
  int num_constraints;
};

typedef struct _displacement
{
  int x, y;
} displacement;

//Physics simulation step data for use in post-step callback.
typedef struct _morphdata {
  int to;
} morphdata;

//Data
static cpVect _verts[VERTICES_MAX];

//Custom collision handlers
#include <handlers.h>

//Initialize Physics Engine.
int
physics_init ()
{
  cpInitChipmunk ();
  log_printf ("Initialized Physics: Chipmunk.");
  return 0;
}

//Quit Physics Engine.
void
physics_quit ()
{
  log_printf ("Physics: Chipmunk UnInitialized.");
}

//Helper Function for Chipmunk Physics
int
physics_collision_begin (cpArbiter * arb, struct cpSpace *space, void *data)
{
	cpShape *a, *b;
	chipmunkobject *cpa, *cpb;
	object *obja, *objb;
	cpArbiterGetShapes(arb, &a, &b);
	cpa = cpShapeGetUserData(a);
	cpb = cpShapeGetUserData(b);
	if(!cpa || !cpb)
		return cpTrue;
	obja = (object*) cpa->omdata;
	objb = (object*) cpb->omdata;
	if(!obja || !objb)
		return cpTrue;
	om_interact(obja, objb, data);
  return cpTrue;
}

//Helper Function for Chipmunk Physics
int
physics_collision_presolve (cpArbiter * arb, struct cpSpace *space,
			    void *data)
{
  return cpTrue;
}

//Helper Function for Chipmunk Physics
void
physics_collision_postsolve (cpArbiter * arb, struct cpSpace *space,
			     void *data)
{
}

//Helper Function for Chipmunk Physics
void
physics_collision_separate (cpArbiter * arb, struct cpSpace *space,
			    void *data)
{
}

//Level geometry via attrmap
void
physics_load_attributemap (void *physics, void *data)
{
  attrmap *am = (attrmap *) data;
  levelphysics *phy = (levelphysics *) physics;
  int l, i, j;
  int *amdata, A, tilewidth, tileheight;
  cpFloat x0, y0, x1, y1, x2, y2, x3, y3;
  int count;
  if (NULL == am)
    return;
  if (NULL == physics)
    return;
  count = 0;
  amdata = am->data;
  tilewidth = am->tilewidth;
  tileheight = am->tileheight;
  if (am->data)
    for (l = 0, i = 0; i < am->height; ++i)
      {
	for (j = 0; j < am->width; ++j, ++amdata)
	  {
	    A = (*amdata);
	    if (-1 == A)
	      {
		++count;
		continue;
	      }
//A = BR*8 + BL*4 + TR*2 + TL --> from Mappy
//A = S*8 + W*4 + E*2 + N --> our interpretation
	    x0 = tilewidth * j;
	    y0 = tileheight * i;
	    x1 = x0 + tilewidth;
	    y1 = y0;
	    x2 = x1;
	    y2 = y1 + tileheight;
	    x3 = x0;
	    y3 = y2;
	    if (A & 1)
	      add_box (phy, x0, y0, x1, y1 + THICK);
	    A >>= 1;		//Top
	    if (A & 1)
	      add_box (phy, x1 - THICK, y1, x2, y2);
	    A >>= 1;		//Right
	    if (A & 1)
	      add_box (phy, x3, y3 - THICK, x2, y2);
	    A >>= 1;		//Bottom
	    if (A & 1)
	      add_box (phy, x0, y0, x3 + THICK, y3);
	    A >>= 1;		//Left
	    if (A & 1)
	      physics_addwater (phy, x0, y0, x2, y2);
	    A >>= 1;		//Water
	  }
      }
  //physics_addwater(phy,0,440,640,460);
#ifdef VERBOSE
  log_printf ("Attrmap had %d empty values out of %d.", count,
	      am->width * am->height);
#endif //VERBOSE
}

//Physics Start.
void *
physics_start (void *params)
{
  levelphysics *chipmunk;

#ifdef VERBOSE
  log_printf ("Starting Physics ...");
#endif //VERBOSE

  chipmunk = (levelphysics *) malloc (sizeof (levelphysics));
  if (NULL == chipmunk)
    return chipmunk;

  //Re-use old level's shape IDs
  cpResetShapeIdCounter();

  chipmunk->space = cpSpaceNew ();

  {
    float g = GRAVITY_EARTH;
    physics_setparam (chipmunk, PHYSICS_PARAM_GRAVITY, &g);
  }

  cpSpaceSetDamping(chipmunk->space,DEFAULT_DAMPING);
  cpSpaceSetSleepTimeThreshold(chipmunk->space, 0.5f);

//Enable contact graph
  cpSpaceSetEnableContactGraph (chipmunk->space, cpTrue);

  cpSpaceSetIterations (chipmunk->space, 30);

  cpSpaceSetCollisionSlop (chipmunk->space, COLLISION_SLOP);

  constraint_list_create( &(chipmunk->constraints), MAX_CHIPMUNK_CONSTRAINTS );
  chipmunk->num_constraints = 0;

//TODO: Spatial Hash? Requires Tuning.
  //cpSpaceUseSpatialHash(chipmunk->space,128,MAX_CHIPMUNK_OBJECTS);

//TODO: Override default collision handlers with our own
  cpSpaceSetDefaultCollisionHandler(chipmunk->space,physics_collision_begin,physics_collision_presolve,physics_collision_postsolve,physics_collision_separate,NULL);

  cpSpaceAddCollisionHandler(chipmunk->space, OBJECT_TRIGGER, OBJECT_PLAYER, disable_first_begin, NULL, NULL, NULL, NULL);
  cpSpaceAddCollisionHandler(chipmunk->space, OBJECT_ITEM, OBJECT_PLAYER, disable_first_begin, NULL, NULL, NULL, NULL);

  chipmunk->_num_objects = 0;
  chipmunk->_elapsed = 0.0f;

#ifdef VERBOSE
  log_printf ("Physics Started.");
#endif //VERBOSE

  return chipmunk;
}

void
physics_cleanup_object(void *physics, void *cpobjptr)
{
  levelphysics *chipmunk = (levelphysics *) physics;
  chipmunkobject *cpobj;
  int s;
  if(NULL == chipmunk || NULL == cpobjptr)
    return;
  cpobj = (chipmunkobject *) cpobjptr;
  if(NULL == cpobj || NULL == cpobj->bodies)
    return;
  if(NULL != cpobj->forms)
  {
    for (s = 0; s < cpobj->forms->size; ++s)
    {
      if(NULL == cpobj->forms->list[s])
        continue;
      physics_cleanup_shape (chipmunk, cpobj->forms->list[s], NULL);
      cpobj->forms->list[s] = NULL;
    }
    shape_list_cleanup (&cpobj->forms);
//    cpobj->forms = NULL;
  }
  for (s = 0; s < cpobj->bodies->size; ++s)
  {
    if (cpobj->bodies->list[s] && !cpBodyIsStatic (cpobj->bodies->list[s]))
      physics_cleanup_body (chipmunk, cpobj->bodies->list[s], NULL);
    cpobj->bodies->list[s] = NULL;
  }
  body_list_cleanup (&cpobj->bodies);
//  cpobj->bodies = NULL;
}

//Physics End.
void
physics_end (void *physics)
{
  levelphysics *chipmunk = (levelphysics *) physics;
#ifdef VERBOSE
  log_printf ("Ending Physics ...");
#endif //VERBOSE
  if (NULL != chipmunk)
    {
      int b;
      for (b = 0; b < chipmunk->_num_objects; ++b)
	{
          chipmunkobject *cpobj = &chipmunk->_objects[b];
          physics_cleanup_object(physics, cpobj);
	}
      if (chipmunk->space)
	{
	  cpSpaceDestroy (chipmunk->space);
	}
      chipmunk->space = NULL;
      chipmunk->_num_objects = 0;
      constraint_list_cleanup(&chipmunk->constraints);
      chipmunk->num_constraints = 0;
      free (chipmunk);
      chipmunk = NULL;
    }
#ifdef VERBOSE
  log_printf ("Physics Ended.");
#endif //VERBOSE
}

void*
on_land_state_change(object *obj, void *data) {
	chipmunkobject *cpobj;
	if(!obj)
		return NULL;
	cpobj = (chipmunkobject *) obj->physicsdata;
	if(!cpobj)
		return obj;
	if(DIR_NONE == cpobj->directionInput)
		sprite_set_state (obj->sprinstance, SPRITESET_IDLE);
	else
		sprite_set_state (obj->sprinstance, SPRITESET_WALK);
	return (void *) obj;
}

//Velocity integration: instead of impulse-based control for platformer-like behaviour
void
physics_bodyvel (cpBody * body, cpVect gravity, cpFloat damping, cpFloat dt)
{
  int s;
  chipmunkobject *cpobj = cpBodyGetUserData (body);
  if (NULL == cpobj)
    {
      cpBodyUpdateVelocity (body, gravity, damping, dt);
      return;
    }
  else
    {
      object *obj = (object *) (cpobj->omdata);
      if (NULL == obj)
	{
	  cpBodyUpdateVelocity (body, gravity, damping, dt);
	  return;
	}
      if ( (obj->type == OBJECT_PLAYER) || (obj->type == OBJECT_ENEMY) )
	{
	  cpFloat target_vx;
	  cpVect surface_v;
	  cpFloat landing_value;
	  shape_list *form;

// Get the grounding information.
	  update_grounding_context (body, &cpobj->grounding);

//TODO: We landed --> sounds? particles?
	  landing_value = cpfabs (cpobj->grounding.impulse.y) * body->m_inv;
//If landed and valid callback exists, call it
//TODO: Tune threshold for grounding impulse
	  if (landing_value > 5.0)
	    {
              cpSpaceAddPostStepCallback(body->CP_PRIVATE(space), (cpPostStepFunc)postStepDispatch, cpobj->forms->list[0], on_land_state_change);
//    		  if(obj->handlers._landed)
              //(*obj->handlers._landed) (obj, NULL);
//              cpSpaceAddPostStepCallback(body->CP_PRIVATE(space), (cpPostStepFunc)postStepDispatch, cpobj->forms->list[0], obj->handlers._landed);
	    }

// Reset jump boosting if you hit your head.
	  if (cpobj->grounding.normal.y < 0.0f)
	    cpobj->remainingBoost = 0.0f;

// Perform a normal-ish update
	  {
	    int jumpState = cpobj->jumpInput;
	    cpBool boost = (jumpState && cpobj->remainingBoost > 0.0f);
	    cpBodyUpdateVelocity (body, (boost ? cpvzero : gravity), damping,
				  dt);
	  }

// Target horizontal velocity used by air/ground control
	  target_vx =
	    obj->vx_desired * ((cpobj->directionInput & DIR_RIGHT ? 1 : 0) -
			       (cpobj->directionInput & DIR_LEFT ? 1 : 0));

// Update the surface velocity and friction
	  surface_v = cpv (target_vx, 0.0f);
          form = cpobj->forms;
	  for (s = 0; s < form->size; ++s)
	    {
              if(!form->list || !form->list[s])
		continue;	
	      form->list[s]->surface_v = surface_v;
	      form->list[s]->u = (cpobj->grounding.body ? PLAYER_GROUND_ACCEL /
		 GRAVITY_EARTH : 0.0);
	      if (cpobj->grounding.body)
		{
		  form->list[s]->u =
		    PLAYER_GROUND_ACCEL / gravity.y;

		  //TODO: Implement separate head and feet for top collisions and bottom collisions
		  //cpobj->headShape->u = HEAD_FRICTION;
		}
	      else
		{
		  form->list[s]->u = 0.0f;
		  //cpobj->headShape->u = 0.0;
		}
	    }

// Apply air control if not grounded
	  if (!cpobj->grounding.body)
	    {
// Smoothly accelerate the velocity
	      body->v.x = cpflerpconst (body->v.x, target_vx,	//+ cpobj->groundVelocity.x,
					PLAYER_AIR_ACCEL * dt);
	    }

// Decrement the jump boosting
	  cpobj->remainingBoost -= dt;

// Bounds on player velocity
	  body->v.y = cpfclamp (body->v.y, -INFINITY, FALL_VELOCITY);
	}
      else
	{
	  cpBodyUpdateVelocity (body, gravity, damping, dt);
	}
    }
}

void
physics_reset_object(void *cpobj)
{
  chipmunkobject *obj = (chipmunkobject *) cpobj;
  if(NULL == obj)
    return;
  obj->bodies = NULL;
  obj->constraints = NULL;
  obj->forms = NULL;
  obj->groundingShape = 0;
  obj->directionInput = 0;
  obj->jumpInput = 0;
  obj->groundVelocity = cpvzero;
  obj->remainingBoost = 0;	//JUMP_BOOST_HEIGHT;
  obj->remainingAirJumps = 0;
  obj->remainingJumpLeniency = JUMP_LENIENCY;
  obj->canDoubleJump = 1;
  obj->grounding.body = NULL;
  obj->lastJumpState = cpFalse;
}

//Create an Object in Physics
void *
physics_create_object (void *physics, void *omdata)
{
  levelphysics *chipmunk = (levelphysics *) physics;
  chipmunkobject *obj;
  object *om_obj = (object *) omdata;
  if (!chipmunk || !om_obj)
    return NULL;
  if (MAX_CHIPMUNK_OBJECTS == chipmunk->_num_objects)
    return NULL;
  
  //New object
  obj = &(chipmunk->_objects[chipmunk->_num_objects]);
  physics_reset_object(obj);
  obj->omdata = omdata;
  ++(chipmunk->_num_objects);

  return (void *) obj;
}

/*
//Attach a body to the object for collision dynamics
void
physics_add_body(void *physics, void *om_obj)
{
  levelphysics *chipmunk = (levelphysics *) physics;
  object *omobj = (object *) om_obj;
  cpSpace *space;
  chipmunkobject *obj;
  cpFloat x, y, w, h;
  if(NULL == chipmunk || NULL == omobj)
    return;

  space = (cpSpace *) chipmunk->space;
  if(NULL == space)
    return;

  obj = (chipmunkobject *) omobj->physicsdata;
  if(NULL == obj)
  {
    omobj->physicsdata = physics_create_object(physics, om_obj);
    obj = (chipmunkobject *) omobj->physicsdata;
  }
  if(NULL == obj)
    return;

  //Retrieve properties from ObjectManager
  x = (cpFloat) omobj->x;
  y = (cpFloat) omobj->y;
  w = (cpFloat) omobj->w;
  h = (cpFloat) omobj->h;

  //Create body (for dynamic equations, e.g. gravity and momentum)
  obj->mass = (cpFloat) DEFAULT_MASS *FLUID_DENSITY * w * h;
  if (omobj->canrotate)
    obj->moment = cpMomentForBox (obj->mass, w, h);
  else
    obj->moment = INFINITY;

  if (omobj->isstatic)
  {
    body_list_create (&obj->bodies, 1);
    body_list_set (obj->bodies, 0, cpSpaceGetStaticBody(space));
    //obj->body = cpSpaceGetStaticBody (space);
    //obj->body = cpBodyNewStatic();
    //obj->body = cpSpaceAddBody (chipmunk->space, cpBodyNew (INFINITY, INFINITY));
  }
  else
  {
    body_list_create (&obj->bodies, 1);
    body_list_set (obj->bodies, 0, cpBodyNew (obj->mass, obj->moment));
    cpBodySetPos (obj->bodies->list[0], cpv (x, y));
    cpBodySetVelLimit (obj->bodies->list[0], cpfmax (PLAYER_VELOCITY, FALL_VELOCITY));
    //cpBodySetVel(obj->body,cpv(0,0));
    obj->bodies->list[0]->velocity_func = physics_bodyvel;
    obj->bodies->list[0] = cpSpaceAddBody (space, obj->bodies->list[0]);
  }
  //Store pointer to chipmunk object in the body, retrieved in callbacks
  cpBodySetUserData (obj->bodies->list[0], obj);
}
*/

//Attach a body to the object for collision dynamics
void
physics_add_bodies(void *physics, void *om_obj)
{
  levelphysics *chipmunk = (levelphysics *) physics;
  object *omobj = (object *) om_obj;
  cpSpace *space;
  chipmunkobject *obj;
  cpFloat x, y, w, h;
  int i;
  if(NULL == chipmunk || NULL == omobj)
    return;

  space = (cpSpace *) chipmunk->space;
  if(NULL == space)
    return;

  obj = (chipmunkobject *) omobj->physicsdata;
  if(NULL == obj)
  {
    omobj->physicsdata = physics_create_object(physics, om_obj);
    obj = (chipmunkobject *) omobj->physicsdata;
  }
  if(NULL == obj)
    return;

  //Retrieve properties from ObjectManager
  x = (cpFloat) omobj->x;
  y = (cpFloat) omobj->y;
  w = (cpFloat) omobj->w;
  h = (cpFloat) omobj->h;

  if(NULL == obj->bodies)
  {
    float brot = 0.0f, mass = 1.0f;
    int isstatic = omobj->isstatic;
    physics_create_body_list (chipmunk, omobj, 1);
    physics_set_body_list (chipmunk, omobj, 0, isstatic, x, y, brot, mass, w, h);
    #ifdef VERBOSE
    log_printf("[PHY] Created %s body at %f, %f", isstatic?"STATIC":"DYNAMIC", x, y);
    #endif //VERBOSE
  }
  if(NULL != obj->bodies)
  {
    for(i=0; i<obj->bodies->size; ++i)
    {
      cpBodySetVelLimit (obj->bodies->list[i], cpfmax (PLAYER_VELOCITY, FALL_VELOCITY));
      obj->bodies->list[i]->velocity_func = physics_bodyvel;
      if(!cpBodyIsStatic(obj->bodies->list[i]))
        obj->bodies->list[i] = cpSpaceAddBody (space, obj->bodies->list[i]);
      //Store pointer to chipmunk object in the body, retrieved in callbacks
      cpBodySetUserData (obj->bodies->list[i], obj);
    }
  }
  if(NULL != obj->constraints)
  {
    for(i=0; i<obj->constraints->size; ++i)
    {
      if(NULL != obj->constraints->list[i])
      {
        //cpSpaceAddConstraint (space, obj->constraints->list[i]);
      }
    }
  }

}

void physics_dump_info (void *physics, void *omdata)
{
  levelphysics *chipmunk = (levelphysics *) physics;
  object *o = (object *) omdata;
  chipmunkobject *co = (chipmunkobject *) o->physicsdata;
  int b = 0, s = 0;
  if(!co)
	return;
  if(NULL != co->bodies)
    b = co->bodies->size;
  if(NULL != co->forms)
    s = co->forms->size;
  log_printf("[PHY] Object \"%s\" has %d bodies and %d shapes.",o->name,b,s);
}

//Attach shapes to the object for collision callbacks
//NOTE: objects with valid shapes, but NULL bodies are desirable, e.g. triggers/sensors.
//      In that case, call only physics_add_shapes() and not physics_add_body().
void
physics_add_shapes (void *physics, void *omobj)
{
  levelphysics *chipmunk = (levelphysics *) physics;
  object *om_obj = (object *) omobj;
  chipmunkobject *obj;
  cpFloat x, y, w, h;
  int i;
  shape **sptr = NULL;

  if (NULL == chipmunk || NULL == om_obj)
    return;
  obj = (chipmunkobject *) om_obj->physicsdata;
  if(NULL == obj)
    return;

  //Retrieve properties from ObjectManager
  x = (cpFloat) om_obj->x;
  y = (cpFloat) om_obj->y;
  w = (cpFloat) om_obj->w;
  h = (cpFloat) om_obj->h;

  if(NULL == obj->forms)
  {
    //If no custom shapes were loaded, use a rectangular bounding box
    shape *pshape;
    shape_list_create (&obj->forms, 1);
    if (om_obj->isstatic)
    {
      //For static bodies, absolute coords are used,
      //  i.e. (0,0) is Center of Mass
      cpShape *pshape = NULL;
      cpVect verts[4];
      verts[0] = cpv(x - w/2.0, y - h/2.0);
      verts[1] = cpv(x - w/2.0, y + h/2.0);
      verts[2] = cpv(x + w/2.0, y + h/2.0);
      verts[3] = cpv(x + w/2.0, y - h/2.0);
      pshape = cpPolyShapeNew (obj->bodies->list[0], 4, verts, cpvzero);
    }
    else
    {
      //For non-static bodies, relative coords are used,
      //  i.e. its Body determines Center of Mass
      //  pshape = cpBoxShapeNew (obj->bodies->list[0], w, h);
      int v;
      cpVect verts[NV];
      cpVect center = cpv(0.0f,0.0f);
      cpFloat rad = (h<w)?h*0.5f:w*0.5f;
      cpFloat angle = 0.0f, angle_step = 2*M_PI / NV;
      for(v=0;v<NV;++v,angle-=angle_step) {
    	  verts[v] = cpvadd(center,cpv(rad*cos(angle),rad*sin(angle)));
      }
      pshape = cpPolyShapeNew (obj->bodies->list[0], NV, verts, cpvzero);
    }
    shape_list_set (obj->forms, 0, pshape);
  }

  for(i=0; i<obj->forms->size; ++i)
  {
    shape *sh = obj->forms->list[i];
    if((NULL != sh) && (NULL != sh)) {
      cpShapeSetElasticity (sh, DEFAULT_ELASTICITY);
      cpShapeSetFriction (sh, DEFAULT_FRICTION);
      if(NULL == sh->body)
      {
        sh->body = cpSpaceGetStaticBody(chipmunk->space);
#ifdef VERBOSE
        log_printf("[PHY] Warning: Object \"%s\" had unattached shapes, which have been added to the space's static body.",om_obj->name);
#endif //VERBOSE
      }
      sh = cpSpaceAddShape (chipmunk->space, sh);
      cpShapeSetCollisionType(sh, om_obj->type);
      //Store pointer to chipmunk object in the shape, retrieved in callbacks
      cpShapeSetUserData (sh, obj);
    }
  }
}

//Set Physics Parameters
void
physics_setparam (void *physics, int paramid, void *value)
{
  levelphysics *chipmunk = (levelphysics *) physics;
  if (NULL == chipmunk)
    return;
  switch (paramid)
    {
    case PHYSICS_PARAM_GRAVITY:
      {
	float given = *(float *) value;
	cpFloat y = (cpFloat) given;
	cpVect gravity = cpv (0, y);
	cpSpaceSetGravity (chipmunk->space, gravity);
	break;
      }
    }
}

//State-specific velocity integration for player state: {jumping, running=left|right|none}
void
physics_update_player (cpBody * body, float dt)
{
  chipmunkobject *obj = (chipmunkobject *) cpBodyGetUserData (body);
  object *omobj;
  int jumpState, jump;
  float time_remaining = dt;
  if (NULL == obj)
    return;
  omobj = (object *) obj->omdata;
  if ((NULL == omobj) || (omobj->type != OBJECT_PLAYER))
    return;
  jumpState = obj->jumpInput;
  //Fixes "Freezing" or "stuck" bug by preventing player body from going to sleep
  if(obj->jumpInput || (obj->directionInput == DIR_LEFT) || (obj->directionInput == DIR_RIGHT)) {
	  int b;
	  for(b=0; b<obj->bodies->size; ++b)
		  cpBodyActivate(obj->bodies->list[b]);
  }
  while(time_remaining >= SIM_DT)
  {
    obj->remainingJumpLeniency -= SIM_DT;
    time_remaining -= SIM_DT;
    obj->wellGrounded = (obj->grounding.body
        &&
        (cpfabs
         (obj->grounding.normal.x / obj->grounding.normal.y) <
         obj->forms->list[obj->groundingShape]->u));
    //obj->wellGrounded = (obj->grounding.body!=NULL);
    if (obj->wellGrounded)
    {
      obj->jumpInput = 0;
      //obj->groundVelocity = obj->grounding.body->v;
      obj->remainingAirJumps = (obj->canDoubleJump) ? 1 : 0;
      obj->remainingJumpLeniency = JUMP_LENIENCY;
    }
    jump = (jumpState && !obj->lastJumpState);

    if (jump
        && (obj->wellGrounded || obj->remainingAirJumps
          || (obj->remainingJumpLeniency > 0)))
    {
      cpFloat jump_v = cpfsqrt (2.0f * JUMP_HEIGHT * GRAVITY_EARTH);
      obj->bodies->list[0]->v = cpvadd (obj->bodies->list[0]->v, cpv (0.0, -jump_v));	//obj->groundVelocity.y - jump_v;

      obj->remainingBoost = JUMP_BOOST_HEIGHT / jump_v;

      //TODO: Jump sound? particles?

      if (!obj->wellGrounded && (obj->remainingJumpLeniency <= 0))
      {			// was a float jump
        obj->remainingAirJumps--;

        // if the player is holding a direction while float jumping, jump in that direction
        // TODO: need to take grounding velocity into account?
        if (obj->directionInput & DIR_LEFT)
          obj->bodies->list[0]->v.x = -PLAYER_VELOCITY;
        if (obj->directionInput & DIR_RIGHT)
          obj->bodies->list[0]->v.x = PLAYER_VELOCITY;
        if (obj->directionInput & DIR_LEFT & DIR_RIGHT)
          obj->bodies->list[0]->v.x = 0;

        //TODO: Double jump sound? particles?
      }

    }
    obj->remainingBoost -= SIM_DT;
    obj->lastJumpState = jumpState;
  }
}

//Update Physics Frames
void
physics_update (void *physics, float dt)
{
  levelphysics *chipmunk = (levelphysics *) physics;
  if (NULL == chipmunk)
    return;
  chipmunk->_elapsed += dt;
  while (chipmunk->_elapsed > SIM_DT)
    {
//Object-specific updates
      unsigned int i, N;
      chipmunkobject *objptr;
      object *ob;
      N = chipmunk->_num_objects;
      objptr = chipmunk->_objects;
      for (i = 0; i < N; ++i, ++objptr)
	{
	  ob = (object *) (objptr->omdata);
	  if(ob->garbage)
		  continue;
	  if (ob->type == OBJECT_PLAYER)
	    physics_update_player (objptr->bodies->list[0], SIM_DT);
	}
      cpSpaceStep (chipmunk->space, SIM_DT);
//Update time
      chipmunk->_elapsed -= SIM_DT;
    }
}


//Get Position, Rotation of Object
void
physics_getpose (void *physics, void *data, float *vector3)
{
  levelphysics *chipmunk = (levelphysics *) physics;
  chipmunkobject *ptr;
  cpVect vector2;
  if (NULL == chipmunk)
    return;
  ptr = (chipmunkobject *) data;
  if(NULL == ptr)
	return;
  //Static body => BodyPose=(0,0)
  //correct pose is retrieved from ObjectManager
  if(cpBodyIsStatic(ptr->bodies->list[0])) {
      object *obj = (object *) (ptr->omdata);
      vector2 = cpv(obj->x, obj->y);
  }
  else {
  //Compensate for collisions
  if (NULL != ptr->grounding.body)
    {
      vector2 =
	cpvadd (cpBodyGetPos (ptr->bodies->list[0]),
		cpvmult (ptr->grounding.normal,
			 ptr->grounding.penetration - COLLISION_SLOP));
    }
  else
    {
      vector2 = cpBodyGetPos (ptr->bodies->list[0]);
    }
  }
  vector3[0] = vector2.x;
  vector3[1] = vector2.y;
  vector3[2] = cpBodyGetAngle (ptr->bodies->list[0]);
}

//Get Linear, Angular velocity of Object
void
physics_getvel (void *physics, void *data, float *vector3)
{
  levelphysics *chipmunk = (levelphysics *) physics;
  chipmunkobject *ptr;
  cpVect vector2;
  if (NULL == chipmunk)
    return;
  ptr = (chipmunkobject *) data;
  if(NULL == ptr)
    return;
  vector2 = ptr->bodies->list[0]->v;
  vector3[0] = vector2.x;
  vector3[1] = vector2.y;
  vector3[2] = ptr->bodies->list[0]->w;
}

//Change state to jump
void
physics_setjump (void *physics, void *data, int jump)
{
  levelphysics *chipmunk = (levelphysics *) physics;
  chipmunkobject *ptr = (chipmunkobject *) data;
  if ((NULL == chipmunk) || (NULL == ptr))
    return;
  ptr->jumpInput = jump;
}

//Change direction (none/left/right)
void
physics_setdirection (void *physics, void *data, int dir)
{
  levelphysics *chipmunk = (levelphysics *) physics;
  chipmunkobject *ptr = (chipmunkobject *) data;
  if (NULL == chipmunk)
    return;
  if (NULL == ptr)
    return;
  ptr->directionInput = dir;
}

//Get direction (none/left/right)
int
physics_getdirection (void *physics, void *data)
{
  levelphysics *chipmunk = (levelphysics *) physics;
  chipmunkobject *ptr = (chipmunkobject *) data;
  if (NULL == chipmunk)
    return DIR_NONE;
  return ptr->directionInput;
}


//Custom collision handler for special "water" tiles
void
physics_addwater (void *physics, float x1, float y1, float x2, float y2)
{
  levelphysics *chipmunk = (levelphysics *) physics;
  cpBody *staticbody;
  cpShape *shape;
  cpBB bb;
  if (NULL == chipmunk)
    return;
  staticbody = cpSpaceGetStaticBody (chipmunk->space);
  bb =
    cpBBNew (cpfmin (x1, x2), cpfmin (y1, y2), cpfmax (x1, x2),
	     cpfmax (y1, y2));
// Add the sensor for the water.
  shape = cpSpaceAddShape (chipmunk->space, cpBoxShapeNew2 (staticbody, bb));
  cpShapeSetSensor (shape, cpTrue);
  cpShapeSetCollisionType (shape, 1);
  cpSpaceAddCollisionHandler (chipmunk->space, 1, 0, NULL,
			      (cpCollisionBeginFunc) waterPreSolve, NULL,
			      NULL, NULL);
}

//Shape visitor, e.g. for debugging
void
physics_visit_shapes (void *physics, void (*shape_visitor) (void *, void *),
		      void *data)
{
  levelphysics *chipmunk = (levelphysics *) physics;
  cpSpaceEachShape (chipmunk->space, (cpSpaceShapeIteratorFunc) shape_visitor,
		    data);
}

//Notify Physics Engine to Update Shape
void
physics_shape_changed (void *physics, void *objptr)
{
  levelphysics *chipmunk = (levelphysics *) physics;
  object *obj = (object *) objptr;
  spriteinstance *givsi = NULL;
  if (NULL == chipmunk || NULL == obj)
    return;
  givsi = obj->sprinstance;
  if (NULL == givsi)
    return;
}

//Render Physics
void
render_physics (ALLEGRO_BITMAP *bitmap, void *physics, int stx, int sty)
{
  displacement d;
  d.x = stx;
  d.y = sty;
  physics_visit_shapes (physics, render_polygon, &d);
}

void
physics_set_shape_list (void *phys, void *omdata, int nvert, float *xvert, float *yvert, int which, int b)
{
  object *obj = (object *) omdata;
  chipmunkobject *cpobj = (chipmunkobject *) obj->physicsdata;
  levelphysics *phy = (levelphysics *) phys;
  cpShape *pshape;
  body *bod;
  cpVect com;
  float *x = xvert, *y = yvert;
  int i;
#ifdef DEBUG
  log_printf("[PHY] Going to add %d verts",nvert);
#endif //DEBUG
  if(NULL == phy || NULL == obj || NULL == cpobj || (nvert <= 0) || NULL == xvert || NULL == yvert || NULL == cpobj->forms)
    return;
#ifdef DEBUG
  log_printf("[PHY] Adding %d verts",nvert);
#endif //DEBUG
  for(i=0; i<nvert; ++i, ++x, ++y) {
    _verts[i] = cpv((cpFloat)(*x), (cpFloat)(*y));
  }
  //Note: at this point, body could be NULL and this is acceptable for shapes not attached to bodies yet
  switch(b)
  {
    case -1:
      bod = cpSpaceGetStaticBody(phy->space);
      break;
    case -2:
      bod = NULL;
      break;
    default:
      bod = cpobj->bodies->list[b];
      break;
  }
  com = cpBodyGetPos(bod);
#ifdef DEBUG
  {
    log_printf("[PHY] Adding shape to body #%d/%d (0x%x) center %f,%f",b,cpobj->bodies->size,bod,com.x,com.y);
  }
#endif //DEBUG

  if(obj->isstatic)
        pshape = cpPolyShapeNew (bod, nvert, _verts, cpv(obj->x, obj->y));
  else
        pshape = cpPolyShapeNew (bod, nvert, _verts, cpvzero);
  //Shapes within the same group do not generate collisions
  cpShapeSetGroup(pshape, obj->id);
  shape_list_set(cpobj->forms, which, pshape);
}

void
physics_set_body_list (void *physics, void *omdata, int which, int isstatic, float bx, float by, float brot, float mass, float sizew, float sizeh)
{
  levelphysics *cp = (levelphysics *) physics;
  object *obj = (object *) omdata;
  chipmunkobject *cpobj = (chipmunkobject *) obj->physicsdata;
  body *bod = NULL;
  cpFloat moment;
  if(obj->canrotate)
    moment = cpMomentForBox (mass, sizew, sizeh);
  else
    moment = INFINITY;
  if(NULL == obj || NULL == cpobj || NULL == cpobj->bodies)
    return;
  if(1 == isstatic)
  {
    bod = cpSpaceGetStaticBody(cp->space);
  }
  else
  {
    bod = cpBodyNew (mass, moment);
    cpBodySetPos (bod, cpv (bx, by));
  }
  body_list_set (cpobj->bodies, which, bod);
}

void
physics_set_constraint_list_global (void *physics, int which, char *type, void *omdataA, void *omdataB, void *cdata)
{
  levelphysics *cp = (levelphysics *) physics;
  object *objA = (object *) omdataA;
  object *objB = (object *) omdataB;
  chipmunkobject *cpobjA = NULL;
  chipmunkobject *cpobjB = NULL;
  constraint_data *cd = (constraint_data *) cdata;
  cpConstraint *cons = NULL;
  body *body_a, *body_b;
  if( (NULL == cd) || (NULL == cp) || (NULL == cp->constraints) || (NULL == type) )
  {
    return;
  }
  if(NULL != objA)
    cpobjA = (chipmunkobject *) objA->physicsdata;
  if(NULL != objB)
    cpobjB = (chipmunkobject *) objB->physicsdata;
  //Use -1 to include the space's default static body in the constraint
  body_a = (-1 == cd->body_a) ? cpSpaceGetStaticBody(cp->space) : cpobjA->bodies->list[cd->body_a];
  body_b = (-1 == cd->body_b) ? cpSpaceGetStaticBody(cp->space) : cpobjB->bodies->list[cd->body_b];
  if(!strcasecmp(type,"PIVOT"))
  {
    cons = cpPivotJointNew( body_a, body_b, cpv(cd->x[0], cd->y[0]) );
  }
  else if(!strcasecmp(type,"SLIDE"))
  {
    cpVect pos_a = cpv(cd->x[0], cd->y[0]);
    cpVect pos_b = cpv(cd->x[1], cd->y[1]);
    /*
    if(-1 == cd->body_a)
    {
      //TODO
      pos_a = cpvadd(pos_a, cpv());
    }*/
    cons = cpSlideJointNew(body_a, body_b,
                pos_a, pos_b,
                cd->min, cd->max );
    //log_printf("Created constraint #%d/%d as 0x%x between body #%d (0x%x) and #%d (0x%x)",which,cpobj->constraints->size,cons,cd->body_a,body_a,cd->body_b,body_b);
  }
  else if(!strcasecmp(type,"ROTARYLIMIT"))
    cons = cpRotaryLimitJointNew(body_a,body_b,cd->min,cd->max);
  constraint_list_set(cp->constraints, which, cons);
  cp->num_constraints = which+1;
}

void
physics_add_constraints_to_space (void *physics)
{
  levelphysics *cp = (levelphysics *) physics;
  int c;
  if(NULL == cp || NULL == cp->constraints)
    return;
  for(c=0; c < cp->num_constraints; ++c)
  {
    if(NULL != cp->constraints->list[c])
      cp->constraints->list[c] = cpSpaceAddConstraint(cp->space, cp->constraints->list[c]);
  }
}

void
physics_set_constraint_list (void *physics, void *omdata, int which, char *type, void *cdata)
{
  levelphysics *cp = (levelphysics *) physics;
  object *obj = (object *) omdata;
  chipmunkobject *cpobj = (chipmunkobject *) obj->physicsdata;
  constraint_data *cd = (constraint_data *) cdata;
  cpConstraint *cons = NULL;
  if(NULL == obj || NULL == cpobj || NULL == cpobj->constraints || NULL == cd)
    return;
  if(!strcasecmp(type,"PIVOT"))
    cons = cpPivotJointNew(cpobj->bodies->list[cd->body_a],cpobj->bodies->list[cd->body_b],cpv(cd->x[0], cd->y[0]));
  else if(!strcasecmp(type,"SLIDE"))
  {
    //Use -1 to include the space's default static body in the constraint
    body *body_a = (-1 == cd->body_a) ? cpSpaceGetStaticBody(cp->space) : cpobj->bodies->list[cd->body_a];
    body *body_b = (-1 == cd->body_b) ? cpSpaceGetStaticBody(cp->space) : cpobj->bodies->list[cd->body_b];
    cpVect pos_a = cpv(cd->x[0], cd->y[0]);
    cpVect pos_b = cpv(cd->x[1], cd->y[1]);
    /*
    if(-1 == cd->body_a)
    {
      //TODO
      pos_a = cpvadd(pos_a, cpv());
    }*/
    cons = cpSlideJointNew(body_a,body_b,
                pos_a,
                pos_b,
                cd->min, cd->max
                );
    log_printf("Created constraint #%d/%d as 0x%x between body #%d (0x%x) and #%d (0x%x)",which,cpobj->constraints->size,cons,cd->body_a,body_a,cd->body_b,body_b);
  }
  else if(!strcasecmp(type,"ROTARYLIMIT"))
    cons = cpRotaryLimitJointNew(cpobj->bodies->list[cd->body_a],cpobj->bodies->list[cd->body_b],cd->min,cd->max);
  constraint_list_set(cpobj->constraints, which, cons);
}

void
physics_attach_shape_to_body (void *physics, void *omdata, int s, int b)
{
  levelphysics *cp = (levelphysics *) physics;
  object *obj = (object *) omdata;
  chipmunkobject *cpobj = (chipmunkobject *) obj->physicsdata;
  if(NULL == obj || NULL == cpobj || NULL == cpobj->bodies || NULL == cpobj->forms)
    return;
  if(NULL == cpobj->forms->list || NULL == cpobj->forms->list[s] || NULL == cpobj->bodies->list || NULL == cpobj->bodies->list[b]|| NULL == cpobj->forms->list[s] || NULL == cpobj->bodies->list[b])
  {
    log_printf("[PHY] Invalid shape-body attachment of shape %d to body %d",s,b);
    return;
  }
  cpShapeSetBody( cpobj->forms->list[s], cpobj->bodies->list[b] );
}

/*
void
physics_morph_form(void *physics, void *cpo, int to)
{
  levelphysics *cp = (levelphysics *) physics;
  chipmunkobject *cpobj = (chipmunkobject *) cpo;
  object *omobj;
  morphdata *data;
  if(NULL == cp)
	return;
  if(NULL == cpobj)
	return;
  omobj  = (object *) cpobj->omdata;
  data = (morphdata *)malloc(sizeof(morphdata));
  if(NULL == omobj || NULL == data)
    return;
  data->to = to;
  cpSpaceAddPostStepCallback(cp->space, (cpPostStepFunc)postStepMorphShape, cpo, (void *)data);
}
*/

//-------------- Internal API ---------------------
DEFINE_LIST(shape);
DEFINE_LIST(body);
DEFINE_LIST(constraint);
DEFINE_WRAPPER_SHAPE_LIST();
DEFINE_WRAPPER_BODY_LIST();
DEFINE_WRAPPER_CONSTRAINT_LIST();

/*
//Custom post-step callback to safely change a shape
static void
postStepMorphShape(cpSpace *space, void *obj, void *data)
{
  chipmunkobject *cpobj = (chipmunkobject *) obj;
  object *omobj = (object *) cpobj->omdata;
  morphdata *mdata = (morphdata *) data;
  int s, from, to;
  shape_list *sl = NULL;
  if(NULL == cpobj || NULL == omobj || NULL == omobj->sprinstance || NULL == mdata)
    return;
  to = mdata->to;
  free(mdata);
  if(SPRITESET_NULL == to)
    return;
  //Disable all shapes of this body
  for(from=0; from<SPRITESET_TYPES; ++from)
  {
    sl = cpobj->forms[from];
    for(s=0; s<sl->size; ++s)
    {
      if(cpSpaceContainsShape(space, sl->list[s]))
        cpSpaceRemoveShape(space, sl->list[s]);
    }
  }
  //Enable only the new one
  sl = cpobj->forms[to];
  for(s=0; s<sl->size; ++s)
  {
    if(!cpSpaceContainsShape(space, sl->list[s]))
      sl->list[s] = cpSpaceAddShape(space, sl->list[s]);
  }
}
*/

//Buoyancy forces using polygon clipping
static cpBool
waterPreSolve (cpArbiter * arb, cpSpace * space, void *ptr)
{
  cpFloat level, clippedArea, dt;
  cpBody *body;
  int count, clippedCount, i, j;
  cpVect *clipped, r, g;
  CP_ARBITER_GET_SHAPES (arb, water, poly);
  body = cpShapeGetBody (poly);

// Get the top of the water sensor bounding box to use as the water level.
  level = cpShapeGetBB (water).t;

// Clip the polygon against the water level
  count = cpPolyShapeGetNumVerts (poly);
  clippedCount = 0;
  clipped = (cpVect *) alloca (sizeof (cpVect) * (count + 1));

  for (i = 0, j = count - 1; i < count; j = i, i++)
    {
      cpVect a = cpBodyLocal2World (body, cpPolyShapeGetVert (poly, j));
      cpVect b = cpBodyLocal2World (body, cpPolyShapeGetVert (poly, i));

      if (a.y > level)
	{
	  clipped[clippedCount] = a;
	  clippedCount++;
	}

      {
	cpFloat a_level = a.y - level;
	cpFloat b_level = b.y - level;

	if (a_level * b_level > 0.0f)
	  {
	    cpFloat t =
	      cpfabs (a_level) / (cpfabs (a_level) + cpfabs (b_level));

	    clipped[clippedCount] = cpvlerp (a, b, t);
	    clippedCount++;
	  }
      }
    }

// Calculate buoyancy from the clipped polygon area
  dt = cpSpaceGetCurrentTimeStep (space);
  g = cpSpaceGetGravity (space);
  clippedArea = cpAreaForPoly (clippedCount, clipped);

  {
    cpFloat displacedMass = clippedArea * FLUID_DENSITY;
    cpVect centroid = cpCentroidForPoly (clippedCount, clipped);
    r = cpvsub (centroid, cpBodyGetPos (body));

// Apply the buoyancy force as an impulse.
    apply_impulse (body, cpvmult (g, -displacedMass * dt), r);
  }

// Apply linear damping for the fluid drag.
  {
    cpVect v_centroid = cpvadd (body->v, cpvmult (cpvperp (r), body->w));
    cpFloat k = k_scalar_body (body, r, cpvnormalize_safe (v_centroid));
    cpFloat damping = clippedArea * FLUID_DRAG * FLUID_DENSITY;
    cpFloat v_coef = cpfexp (-damping * dt * k);	// linear drag
//cpFloat v_coef = 1.0/(1.0 + damping*dt*cpvlength(v_centroid)*k); // quadratic drag
    apply_impulse (body,
		   cpvmult (cpvsub (cpvmult (v_centroid, v_coef), v_centroid),
			    1.0 / k), r);
  }

// Apply angular damping for the fluid drag.
  {
    cpFloat w_damping =
      cpMomentForPoly (FLUID_DRAG * FLUID_DENSITY * clippedArea, clippedCount,
		       clipped, cpvneg (body->p));
    body->w *= cpfexp (-w_damping * dt * body->i_inv);
  }

  return cpTrue;
}

//Helper Function for Chipmunk Physics
static void
physics_cleanup_body (void *physics, cpBody * objptr, void *data)
{
  levelphysics *chipmunk = (levelphysics *) physics;
  if (NULL == chipmunk || objptr == NULL)
    return;
//Single static body is memory-managed by Chipmunk
  if (cpBodyIsStatic (objptr))
    {
      cpBodyDestroy (objptr);
    }
  else
    {
      cpSpaceRemoveBody (chipmunk->space, objptr);
    }
  cpBodyFree (objptr);
}

//Helper Function for Chipmunk Physics
static void
physics_cleanup_shape (void *physics, cpShape * sobjptr,
		       void *data)
{
  levelphysics *chipmunk = (levelphysics *) physics;
  if (NULL == chipmunk || sobjptr == NULL)
    return;
  if(cpSpaceContainsShape(chipmunk->space, sobjptr))
    {
      cpSpaceRemoveShape (chipmunk->space, sobjptr);
    }
}

static void
add_box (levelphysics * chipmunk, cpFloat x1, cpFloat y1, cpFloat x2,
	 cpFloat y2)
{
//common static body to attach segment to
  cpBody *staticBody = cpSpaceGetStaticBody (chipmunk->space);
//Create Geometry (for Shape-Based Collisions)
  cpShape *seg = cpSpaceAddShape (chipmunk->space,
				  cpBoxShapeNew2 (staticBody,
						  cpBBNew (cpfmin (x1, x2),
							   cpfmin (y1, y2),
							   cpfmax (x1, x2),
							   cpfmax (y1, y2))));
  cpShapeSetElasticity (seg, (cpFloat) DEFAULT_ELASTICITY);
  cpShapeSetFriction (seg, (cpFloat) DEFAULT_FRICTION);
}

//Store collision information when a body hits another
//use: player <---> level geometry
static void
callback_grounding_context (cpBody * body, cpArbiter * arb,
			    grounding_context * grounding)
{
  cpVect n;
  CP_ARBITER_GET_BODIES (arb, b1, b2);
  n = cpvneg (cpArbiterGetNormal (arb, 0));

  if (n.y < grounding->normal.y)
    {
      grounding->normal = cpvneg (n);
      grounding->penetration = cpArbiterGetDepth (arb, 0);
      grounding->body = b2;
    }

  grounding->impulse =
    cpvadd (grounding->impulse, cpArbiterTotalImpulseWithFriction (arb));
}

//Iterate over colliding bodies to update grounding information
static void
update_grounding_context (cpBody * body, grounding_context * context)
{
  (*context).normal = cpvzero;
  (*context).impulse = cpvzero;
  (*context).penetration = (cpFloat) 0.0f;
  (*context).body = NULL;
  cpBodyEachArbiter (body,
		     (cpBodyArbiterIteratorFunc) callback_grounding_context,
		     context);
}

static void
render_constraint (cpBody *body, cpConstraint *constraint, void *data)
{
  cpBody *body_a = constraint->a;
  cpBody *body_b = constraint->b;
  int x = (NULL == data)? 0 : ((displacement *) data)->x;
  int y = (NULL == data)? 0 : ((displacement *) data)->y;

  if(cpSlideJointGetClass() == constraint->CP_PRIVATE(klass))
  {
    cpSlideJoint *joint = (cpSlideJoint *) constraint;
    cpVect a = cpvadd(body_a->p, cpvrotate(joint->anchr1, body_a->rot));
    cpVect b = cpvadd(body_b->p, cpvrotate(joint->anchr2, body_b->rot));

    al_draw_filled_circle( a.x-x, a.y-y, 4, al_map_rgb(255,0,0) );
    al_draw_filled_circle( b.x-x, b.y-y, 2, al_map_rgb(0,0,255) );
  }
}

static void
render_polygon (void *shape, void *data)
{
  int n, v, x, y;
  cpVect v1, v2, vcom;
  chipmunkobject *cpobj = (chipmunkobject *) cpShapeGetUserData(shape);
  object *obj;
  body *bod;
  if(NULL == cpobj)
	return;
  obj = (object *) cpobj->omdata;
  n = cpPolyShapeGetNumVerts (shape);
  x = ((displacement *) data)->x;
  y = ((displacement *) data)->y;
  if(n == 0)
    return;
  bod = cpShapeGetBody (shape);
  //body coords are in world frame
  vcom = cpBodyGetPos (bod);
  al_draw_filled_rectangle(vcom.x-3-x,vcom.y-3-y,vcom.x+3-x,vcom.y+3-y,al_map_rgb(0,0,128));
  //shape coords are in body-centered frame
  v1 = cpBodyLocal2World (bod, cpPolyShapeGetVert (shape, 0));
  for (v = 0; v < n; ++v)
  {
    v2 = cpBodyLocal2World (cpShapeGetBody (shape),
        cpPolyShapeGetVert (shape, (v+1)%n ));
    al_draw_line (v1.x - x, v1.y - y, v2.x - x, v2.y - y,
        al_map_rgb (255, 0, 255), 1.0f);
    v1 = v2;
  }
  cpBodyEachConstraint(bod, render_constraint, data);
}

void physics_add_rope (void *physics, void *omdata)
{
  levelphysics *chipmunk = (levelphysics *) physics;
  object *o = (object *) omdata;
  chipmunkobject *co;

  int i, j;
  cpFloat mass = 1;
  cpFloat width = 3;
  cpFloat height = 40;
  cpSpace *space = chipmunk->space;
  cpFloat spacing = 0.2;
  cpBody *body;
  cpShape *shape;
  cpBody *staticBody = cpSpaceGetStaticBody(space);
  int N = CHAIN_COUNT * LINK_COUNT, n = 0;

  physics_create_body_list (physics, o, N);
  physics_create_shape_list (physics, o, N);
  physics_create_constraint_list (physics, o, 2*N);
  co = (chipmunkobject *) o->physicsdata;

  for(i=0; i<CHAIN_COUNT; i++){
    cpBody *prev = NULL;

    for(j=0; j<LINK_COUNT; j++, ++n){
      cpConstraint *constraint = NULL;

      cpVect pos = cpv(300 + 60*(i - (CHAIN_COUNT - 1)/2.0), 80+(j + 0.5)*height);
      //cpVect pos = cpv(200, 20 + (j+0.5)*height);

      //body = cpSpaceAddBody(space, cpBodyNew(mass, cpMomentForBox(mass, width, height)));
      body = cpBodyNew(mass, cpMomentForBox(mass, width, height));
      cpBodySetPos(body, pos);
      co->bodies->list[n] = body;

      //shape = cpSpaceAddShape(space, cpBoxShapeNew(body, width, height));
      shape = cpBoxShapeNew(body, width, height);
      cpShapeSetGroup(shape, i+1);
      cpShapeSetFriction(shape, 0.8f);
      co->forms->list[n] = shape;

      if(prev == NULL){
        //constraint = cpSpaceAddConstraint(space, cpSlideJointNew(body, staticBody, cpv(0, height/2), cpvadd(pos,cpv(0, height)), 0, spacing));
        constraint = cpSlideJointNew(body, staticBody, cpv(0, -height/2), cpvadd(pos,cpv(0, height)), 0, spacing);
        constraint = cpSpaceAddConstraint(space, constraint);
        co->constraints->list[2*n] = constraint;
        //cpSpaceAddConstraint(space, cpRotaryLimitJointNew(body, staticBody, -0.3, 0.3) );
        constraint = cpRotaryLimitJointNew(body, staticBody, -0.3, 0.3);
        constraint = cpSpaceAddConstraint(space, constraint);
        co->constraints->list[2*n +1] = constraint;
      } else {
        constraint = cpSlideJointNew(body, prev, cpv(0, -height/2), cpv(0, height/2), 0, spacing);
        constraint = cpSpaceAddConstraint(space, constraint);
        co->constraints->list[2*n] = constraint;
        //cpSpaceAddConstraint(space, cpRotaryLimitJointNew(body, prev, -0.3, 0.3) );
        constraint = cpRotaryLimitJointNew(body, prev, -0.3, 0.3);
        constraint = cpSpaceAddConstraint(space, constraint);
        co->constraints->list[2*n +1] = constraint;
      }
      prev = body;
    }
  }
}

