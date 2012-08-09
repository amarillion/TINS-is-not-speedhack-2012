#include <log.h>
#include <statedef.h>
#include <script.h>
#include <sprite.h>
#include <level.h>
#include <gameglobal.h>
#include <physics.h>
#include <string.h>
#include <object.h>
#include <camera.h>

//Object type names
const char object_typename_table[OBJECT_TYPES][MAXSTR] = {
  "unknown",
  "player",
  "enemy",
  "item",
  "door",
  "platform",
  "trigger",
  "npc",
  "rope",
};

typedef struct {
	object_pair_handler_func_ptr func;
} interaction_handler;
static interaction_handler interaction_handlers[MAX_OBJECTS][MAX_OBJECTS];

void
om_reset (objectmanager * om)
{
  object **obj;
  int o;
  if(!om)
    return;
//Empty the set
  obj = om->objectset;
  for(o=0;o<MAX_OBJECTS; ++o, ++obj)
  {
    if(*obj)
      free(*obj);
    *obj = NULL;
  }
//Reset level pointer
  om->level = NULL;
//Default: No interactions between object pairs
  {
	  int a, b;
	  for(a=0;a<MAX_OBJECTS;++a) {
		  for(b=0;b<MAX_OBJECTS;++b) {
			  interaction_handlers[a][b].func = (object_pair_handler_func_ptr) NULL;
		  }
	  }
  }
#ifdef VERBOSE
  log_printf ("Reset ObjectManager ...");
#endif //VERBOSE

}

objectmanager *
om_start (level * l)
{
  objectmanager *om = (objectmanager *) malloc (sizeof (objectmanager));
  int o;
  objectptr_iterator obj;
  om->begin = om->objectset;
  om->end = om->begin;
  obj = om->begin;
  for(o=0;o<MAX_OBJECTS; ++o, ++obj)
    *obj = NULL;
  om->level = l;
#ifdef VERBOSE
  log_printf ("Started ObjectManager ...");
#endif //VERBOSE
  return om;
}

void
om_end (objectmanager * om)
{
  if (NULL == om)
    return;
#ifdef VERBOSE
  log_printf ("Ending ObjectManager ...");
#endif //VERBOSE
  om_reset (om);
  free (om);
}

//Create an instance of a sprite
spriteinstance *
om_create_sprite_instance (objectmanager * om, const char *spritename)
{
  spriteinstance *sprinstance = NULL;
  spritemap *sprmap = NULL;
  if(NULL == om)
    return sprinstance;
  if (NULL == om->level)
    return sprinstance;
  sprmap = om->level->sprm;
  if (NULL != sprmap)
    {
      sprite *whichsprite = NULL;
      int s;
//Lookup sprite with matching name
      for (s = 0; s < sprmap->nsprites; ++s)
	{
	  if (!strcasecmp (sprmap->spritenames[s], spritename))
	    {
	      whichsprite = sprmap->sprites[s];
	      break;
	    }
	}
      if (NULL != whichsprite)
	{
	 //Create a sprite instance
	  int curspriteinstance = sprmap->nspriteinstances;
	  sprmap->spriteinstances[curspriteinstance].curframe = 0;
	  sprmap->spriteinstances[curspriteinstance].cursst = DEFAULT_FORM;
	  sprmap->spriteinstances[curspriteinstance].prevsst = DEFAULT_FORM;
	  sprmap->spriteinstances[curspriteinstance].sprite = whichsprite;
	  sprinstance = &sprmap->spriteinstances[curspriteinstance];
	  sprite_set_state (sprinstance, DEFAULT_FORM);
#ifdef VERBOSE
	  log_printf ("[OBJECT] Sprite instance #%d.",sprmap->nspriteinstances);
#endif //VERBOSE
	  sprmap->nspriteinstances++;
	}
      else
	{
#ifdef VERBOSE
	  log_printf ("[OBJECT] Sprite name match not found.");
#endif //VERBOSE
	}
    }
  else
    {
#ifdef VERBOSE
      log_printf ("[OBJECT] Suitable LEVEL_SPRITEMAP not found in Level %d.",
	 curlevel);
#endif //VERBOSE
    }
  return sprinstance;
}

objectmanager *
om_get_active_objectmanager ()
{
#ifdef VERBOSE
  log_printf ("[OBJECT] Current level query: %d", curlevel);
#endif //VERBOSE
  if (NULL != levels[curlevel])
    return (objectmanager *) (levels[curlevel]->objectmanager);
  return NULL;
}

static const object_handler HANDLER_NULL = { NULL, NULL, NULL };

object *
om_create (objectmanager * om)
{
  object *newobject;
  objectptr_iterator avail, end;
  int o = 0;
  if (NULL == om)
    {
      log_printf
	("[OBJECT] NULL instance of ObjectManager received. Should abort.");
      exit (1);
    }
  //Find next available slot for object
  avail = om->begin;
  end = om->end;
  while((avail != end) && (*avail) && (!(*avail)->garbage))
  {
    ++avail;
    ++o;
  }
  //No space available
  if(MAX_OBJECTS == o)
    return NULL;
  if(*avail)
  {
    //Already allocated, so re-use it
    newobject = *avail;
  }
  else
  {
    //Allocate new, update end
    newobject = (object *) malloc (sizeof(object));
    *avail = newobject;
    om->end = avail+1;
  }
  //Pointer to itself used as unique id
  newobject->id = o;
  newobject->garbage = 0;
  newobject->setlocation = o;
  newobject->x = 0.0f;
  newobject->y = 0.0f;
  newobject->angle = 0.0f;
  newobject->vx = 0.0f;
  newobject->vy = 0.0f;
  newobject->vx_desired = 0.0f;
  newobject->vy_desired = 0.0f;
  newobject->wt = 1.0f;
  newobject->sprinstance = NULL;
  newobject->w = 1.0f;
  newobject->h = 1.0f;
  newobject->health = 100.0f;
  newobject->health_max = newobject->health;
  newobject->canrotate = 0;
  newobject->facing = OBJECT_FACING_DEFAULT;
  newobject->isstatic = 0;
  newobject->isrogue = 0;
  newobject->customshapes = 0;
  newobject->physicsdata = NULL;
  newobject->handlers = HANDLER_NULL;
  newobject->type = OBJECT_UNKNOWN;
  newobject->physicsdata = NULL;
  newobject->layer=0;
  newobject->state = 1;
  newobject->omanager = om;
  return newobject;
}

void
om_add (objectmanager * om, object * newobject, void * data)
{
  if(!om || !newobject)
    return;
  //NOTE: obj->w, obj->h are only used once to calculate body mass
  if (NULL != newobject->sprinstance)
  {
    newobject->w = newobject->sprinstance->sprite->spritewidth;
    newobject->h = newobject->sprinstance->sprite->spriteheight;
  }
  //Only add dynamics for known objects
  if(!newobject->isrogue)
  {
#ifdef DEBUG
   log_printf("[OBJECT] Adding bodies of \"%s\" to space...",newobject->name);
#endif //DEBUG
   physics_add_bodies(om->level->physics, newobject);
  //Everything has a shape for collisions
  //TODO: possibly condition this on other types if we don't need to check for collisions at all
#ifdef DEBUG
   log_printf("[OBJECT] Adding shapes to space...");
#endif //DEBUG
   physics_add_shapes (om->level->physics, newobject);
  }
  script_add_to_dictionary (lua, newobject->name, newobject);
  log_printf("[OBJECT] Added %d(%s)",newobject->id,newobject->name);
}

void
om_dump_object (object * ptr)
{
  if (NULL == ptr)
    log_printf
      ("[om_dump_object] Invalid pointer (NULL) received. Ignoring, but this could be a problem.");
  else
    log_printf ("Object info: {%s @(%.1lf,%.1lf) phy(%x)}", ptr->name, ptr->x,
		ptr->y, ptr->physicsdata);
}

void
om_center_camera_on (object * ptr)
{
  if (NULL != ptr)
    {
      center_camera(ptr);
    }
}

object *
om_get_object_by_name (const char *name)
{
  object *found = NULL;
//stack: []
  lua_pushstring (lua, name);
//stack: [string]
  l_FindObjectByName (lua);
//stack: [void *]
  found = (object *) lua_touserdata (lua, -1);
  lua_pop (lua, -1);
//stack: []
  return found;
}

void
om_set_handler (object * ptr, int handler_type,
		void *(*handler) (object * obj, void *data))
{
  if(!ptr || !handler || (handler_type < 0) || (handler_type >= OBJECT_HANDLER_TYPES))
    return;
  {
    typedef void *(*HANDLER) (object * obj, void *data);
    HANDLER *h = (HANDLER *) & ptr->handlers;
    h[handler_type] = handler;
  }
}

//Number of objects
int
om_get_number_of_objects(objectmanager * om)
{
  return (om->end - om->begin);
}

int
om_update (void *omgr, float dt)
{
  objectmanager *om = (objectmanager *) omgr;
  float posvec[3];
  object *objptr = NULL;
  objectptr_iterator objit;
  int updated = 0;
  physics_update (om->level->physics, dt);
  //Retrieve new non-static object poses and speeds for consistency
  for (objit=om->begin; objit!=om->end; ++objit)
  {
    int objupdated = 0;
    objptr = *objit;
    if((NULL == objptr) || objptr->garbage)
      continue;
    if (!objptr->isrogue)
    {
     if(!objptr->isstatic) {
      physics_getpose (om->level->physics, objptr->physicsdata,
          posvec);

      objptr->x = posvec[0];
      objptr->y = posvec[1];
      objptr->angle = posvec[2];
      physics_getvel (om->level->physics, objptr->physicsdata,
          posvec);
      objptr->vx = posvec[0];
      objptr->vy = posvec[1];
      if ((objptr->vx != 0) || (objptr->vy != 0))
        objupdated = 1;
      }
    }
	//Per-object update if defined
	if (NULL != objptr->handlers._func[OBJECT_HANDLER_UPDATE])
	{
		objptr->handlers._func[OBJECT_HANDLER_UPDATE] (objptr, NULL);
		objupdated = 1;
	}
    if(objptr->health <= 0.0) {
    	if(om_get_object_state(objptr) == SPRITESET_DYING) {
    		if(objptr->sprinstance->sprite->spritesets[SPRITESET_DYING] == NULL) {
    			om_remove_object(om, objptr);
    		}
    		else if(objptr->sprinstance->curframe == objptr->sprinstance->sprite->spritesets[SPRITESET_DYING]->nframes - 1) {
    			om_remove_object(om, objptr);
    		}
    	}
    	else
    		om_set_object_state(objptr,SPRITESET_DYING);
    }
    updated += objupdated;
  }
  return updated;
}

//Mark the object as garbage, so it can be re-used
void
om_dispose (objectmanager * om, object * object)
{
  objectptr_iterator marker;
  if(!om || !object)
    return;
  marker = om->objectset + object->setlocation;
  (*marker)->garbage = 1;
}

const char* OBJECT_HANDLER_NAMES[OBJECT_HANDLER_TYPES] = {
		"init",
		"destroy",
		"update",
		"interact"
};

//Removed checking because this function is called many times per frame
CINLINE static int om_get_sequential_id(object *obj) {
	return obj->id;
}

void om_interact(object *obja, object *objb, void *data) {
	int idA, idB, ida, idb;
	object *a, *b;
        object_pair_handler_func_ptr interact;
	idA = om_get_sequential_id(obja);
	idB = om_get_sequential_id(objb);
	//Lex order
	ida = (idA<idB)?idA:idB;
	idb = (idA<idB)?idB:idA;
	a = (idA<idB)?obja:objb;
	b = (idA<idB)?objb:obja;
	interact = interaction_handlers[ida][idb].func;
	if(interact != NULL) {
		log_printf("[OBJECT] Interact Exists for %s, %s.", obja->name, objb->name);
		interact(a,b,data);
	}
	else {
		log_printf("[OBJECT] No Interaction Exists for %s, %s.", obja->name, objb->name);
	}
}

void om_set_interaction(object *obja, object *objb, object_pair_handler_func_ptr func) {
	int ida, idb;
	ida = om_get_sequential_id(obja);
	idb = om_get_sequential_id(objb);
	log_printf("[OBJECT] Adding Interaction for: %s, %s.", obja->name, objb->name);
	interaction_handlers[ida][idb].func = func;
	interaction_handlers[idb][ida].func = func;
}

int
om_get_object_state(object *obj) {
	if(!obj)
		return 0;
	if(!obj->sprinstance)
		return obj->state;
	return sprite_get_state(obj->sprinstance);
}

void
om_set_object_state(object *obj, int state) {
	if(!obj)
		return;
	if(!obj->sprinstance)
		obj->state = state;
	else
		sprite_set_state(obj->sprinstance, state);
	log_printf("[SET_OBJ_STATE] %s => %d",obj->name,sprite_get_state(obj->sprinstance));
}

void
om_remove_object(objectmanager *om, object *obj) {
	if(NULL == obj)
		return;
	if(NULL == obj->physicsdata)
		return;
	//Call the destroy_object callback if it was defined
	if (NULL != obj->handlers._func[OBJECT_HANDLER_DESTROY])
		obj->handlers._func[OBJECT_HANDLER_DESTROY] (obj, NULL);
	script_remove_from_dictionary(obj->name);
	physics_cleanup_object(om->level->physics, obj->physicsdata);
	obj->physicsdata = NULL;
	om_dispose(om, obj);
}
