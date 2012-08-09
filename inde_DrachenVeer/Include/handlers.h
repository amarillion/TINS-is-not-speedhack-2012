#ifndef _HANDLERS_H_
#define _HANDLERS_H_

#include <defs.h>
#include <gameglobal.h>
#include <physics.h>
#include <chipmunk/chipmunk.h>
#include <object.h>
#include <log.h>
#include <level.h>

CINLINE static void
postStepDispatch(cpSpace *space, cpShape *shape, void *unused)
{
  chipmunkobject *cpobj = (chipmunkobject *) cpShapeGetUserData(shape);
  object *omobj = (object *) cpobj->omdata;
  if(unused)
	((object_handler_func_ptr)unused)(omobj, NULL);
}

CINLINE static void
postStepRemoveObject(cpSpace *space, cpShape *shape, void *unused)
{
  chipmunkobject *cpobj = (chipmunkobject *) cpShapeGetUserData(shape);
  object *omobj = (object *) cpobj->omdata;
  chipmunkobject *cpother = NULL;
  object *other = NULL;
  if(NULL == cpobj || NULL == omobj)
    return;
  if (NULL != omobj->handlers._func[OBJECT_HANDLER_DESTROY])
  {
    if(NULL != unused)
    {
      cpother = (chipmunkobject *) cpShapeGetUserData(unused);
      other = (object *) cpobj->omdata;
    }
    omobj->handlers._func[OBJECT_HANDLER_DESTROY] (omobj, other);
  }
  physics_cleanup_object(levels[curlevel]->physics, cpobj);
  omobj->physicsdata = NULL;
  om_dispose(levels[curlevel]->objectmanager, omobj);
}

CINLINE static int
disable_first_begin(cpArbiter *arb, cpSpace *space, void *unused)
{
  cpShape *a, *b;
  cpArbiterGetShapes(arb, &a, &b);
  //mark A for removal
  cpSpaceAddPostStepCallback(space, (cpPostStepFunc)postStepRemoveObject, a, b);
  return 0;
}

CINLINE static int
disable_second_begin(cpArbiter *arb, cpSpace *space, void *unused)
{
  cpShape *a, *b;
  cpArbiterGetShapes(arb, &a, &b);
  //mark B for removal
  cpSpaceAddPostStepCallback(space, (cpPostStepFunc)postStepRemoveObject, b, a);
  return 0;
}

#endif //_HANDLERS_H_
