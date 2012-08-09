#ifndef PHYSICS_CHIPMUNK_H
#define PHYSICS_CHIPMUNK_H

#include "olist.h"

#define _PASTER_PHY_ALLOC_DEFN(name, phyname, member) void \
        physics_create_ ## name ## _list (void *phy, void *omdata, int size) \
        { \
          levelphysics *physics = (levelphysics *) phy; \
          object *obj = (object *) omdata; \
          phyname *cpobj = (phyname *) obj->physicsdata; \
          if(NULL == cpobj) \
          { \
            obj->physicsdata = physics_create_object (physics, obj); \
            cpobj = (phyname *)obj->physicsdata; \
          } \
          if(NULL == obj || NULL == cpobj || (size <= 0)) \
            return; \
          name ## _list_cleanup(&cpobj->member); \
          name ## _list_create(&cpobj->member, size); \
        }
#define _PASTER_PHY_ALLOC_DECL(name) void \
        physics_create_ ## name ## _list (void *phy, void *omdata, int size)

//Wrappers to expose functionality using physics_*()
#define DECLARE_WRAPPER_LIST(name) _PASTER_PHY_ALLOC_DECL(name)
#define DEFINE_WRAPPER_SHAPE_LIST() _PASTER_PHY_ALLOC_DEFN(shape,chipmunkobject,forms)
#define DEFINE_WRAPPER_BODY_LIST() _PASTER_PHY_ALLOC_DEFN(body,chipmunkobject,bodies)
#define DEFINE_WRAPPER_CONSTRAINT_LIST() _PASTER_PHY_ALLOC_DEFN(constraint,chipmunkobject,constraints)

//Wrappers
DECLARE_WRAPPER_LIST(shape);
DECLARE_WRAPPER_LIST(body);
DECLARE_WRAPPER_LIST(constraint);

#endif //PHYSICS_CHIPMUNK_H
