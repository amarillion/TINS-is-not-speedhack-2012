#include <global.h>
#include <object.h>
#include <log.h>
#include <physics.h>
#include <sprite.h>

//Load Shapes from a pre opened file
void *
load_shapes_file (FILE *fshp, objectmanager * om, object * obj, int body, float sx, float sy)
{
  int nvert = 0, nforms = 0;
  int j, f;
  float vx[VERTICES_MAX], vy[VERTICES_MAX];
  float *x = vx, *y = vy;
  if(!fshp || !om || !obj)
    return NULL;
  obj->customshapes = 1;

  fscanf(fshp,"%d",&nforms);

#ifdef DEBUG
  log_printf("[SHP] %d form(s)", nforms);
#endif //DEBUG

  for(f=0; f<nforms; ++f)
  {
    fscanf(fshp,"%d",&nvert);
#ifdef DEBUG
    log_printf("[SHP] %d verts", nvert);
#endif //DEBUG
    x = vx;
    y = vy;
    for(j=0; j<nvert; ++j, ++x, ++y)
    {
      fscanf(fshp,"%f%f",x,y);
      *x += sx;
      *y += sy;
#ifdef DEBUG
      log_printf("[SHP]    %f,%f", *x, *y);
#endif //DEBUG
    }
    physics_set_shape_list (om->level->physics, obj, nvert, vx, vy, f, body);
#ifdef DEBUG
    log_printf("[SHP]    Attached shape to body %d", body);
#endif //DEBUG
  }
  return NULL;
}

//Load ObjectMap from a pre opened file
//TODO: apply rotation offset 'orot'
void *
load_object_template_file (FILE * fobt, char *levelname, objectmanager * om, object *obj, float ox, float oy, float orot)
{
  int i;
  int nobj = 0, type;
  char objtype[MAXSTR], sprname[MAXSTR];
  void *data = NULL;
  int load_sprites, read_shapes;
  int nbodies = 0, nconstraints = 0, nshp = 0, nshapes = 0;

  if (!om || !obj)
    return om;

  if(!fobt)
    return om;

  //Just opened example_object.obt
  load_sprites = 0;
  read_shapes = 0;
  //OBJECT_TYPE

  *objtype = 0;
  *sprname = 0;
  fscanf (fobt, "%s %d %d %d %d %s", objtype, &nbodies, &nconstraints, &nshp, &nshapes, sprname);

  for(type=0; type<OBJECT_TYPES; ++type)
    if(!strcasecmp(objtype, object_typename_table[type]))
      break;

  //Ignore objects with types not found
  if(OBJECT_TYPES == type) {
    log_printf("[OBJECT] Unknown Type Discovered: %s", objtype);
    return om;
  }

#ifdef DEBUG
  log_printf("[OBJECT] Loading %s %d %d %d %d %s", objtype, nbodies, nconstraints, nshp, nshapes, sprname);
#endif //DEBUG

  //Load sprites
  if (strcasecmp ("NULL", sprname))
    obj->sprinstance = om_create_sprite_instance (om, sprname);
  obj->type = type;
  data = NULL;
  obj->isrogue = 0;
  if((nshp + nshapes + nconstraints + nbodies) == 0)
	obj->isrogue = 1;

  if(nbodies > 0)
  {
    // [WORLD_STATIC = 1 | BODY = 0] body_specs
    physics_create_body_list (om->level->physics, obj, nbodies);
    for(i=0; i<nbodies; ++i)
    {
      float bx, by, brot, mass, sw, sh;
      int isstatic;
      fscanf(fobt, "%d%f%f%f%f%f%f",&isstatic,&bx,&by,&brot,&mass,&sw,&sh);
      obj->isstatic = isstatic;
      bx += ox;
      by += oy;
      physics_set_body_list (om->level->physics, obj, i, isstatic, bx, by, brot, mass, sw, sh);
#ifdef DEBUG
      log_printf("[LOAD] Created %s body at %f, %f", isstatic?"STATIC":"DYNAMIC", bx, by);
#endif //DEBUG
#ifdef DEBUG
  physics_dump_info(om->level->physics, obj);
#endif //DEBUG
    }
  }

  if(nconstraints > 0)
  {
    char constraint[MAXSTR];
    constraint_data cd;

    // [PIN | SLIDE | PIVOT | GROOVE | DAMPEDSPRING | DAMPEDROTARYSPRING | ROTARYLIMIT | RATCHET | GEAR | MOTOR] constraint_specifications
    physics_create_constraint_list (om->level->physics, obj, nconstraints);

    for(i=0; i<nconstraints; ++i)
    {
      fscanf(fobt, "%s", constraint);
      if(!strcasecmp(constraint,"SLIDE"))
      {
        fscanf(fobt, "%d%d%f%f%f%f%f%f", &cd.body_a, &cd.body_b, &cd.x[0], &cd.y[0], &cd.x[1], &cd.y[1], &cd.min, &cd.max);
        physics_set_constraint_list (om->level->physics, obj, i, constraint, &cd);
#ifdef DEBUG
        log_printf("[LOAD] Constraint %s", constraint);
#endif //DEBUG
      }
      else if(!strcasecmp(constraint,"PIVOT"))
      {
        fscanf(fobt, "%d%d%f%f", &cd.body_a, &cd.body_b, &cd.x[0], &cd.y[0]);
        physics_set_constraint_list (om->level->physics, obj, i, constraint, &cd);
#ifdef DEBUG
        log_printf("[LOAD] Constraint %s", constraint);
#endif //DEBUG
      }
      else if(!strcasecmp(constraint,"ROTARYLIMIT"))
      {
        fscanf(fobt, "%d%d%f%f", &cd.body_a, &cd.body_b, &cd.min, &cd.max);
        physics_set_constraint_list (om->level->physics, obj, i, constraint, &cd);
#ifdef DEBUG
        log_printf("[LOAD] Constraint %s", constraint);
#endif //DEBUG
      }
    }
#ifdef DEBUG
  physics_dump_info(om->level->physics, obj);
#endif //DEBUG
  }
  if(nshp > 0)
  {
    char shpname[MAXSTR], uvname[MAXSTR];
    physics_create_shape_list(om->level->physics, obj, nshapes);
    for(i=0; i<nshp; ++i)
    {
      float sx, sy, srot;
      int att_body;
      fscanf (fobt, "%d%f%f%f%s%s", &att_body, &sx, &sy, &srot, shpname, uvname);
      //Load custom shapes
      if (strcasecmp ("NULL", shpname))
      {
        FILE *fshp = NULL;
        char fshpname[MAXSTR];
        *fshpname = 0;
        sprintf (fshpname, "%s%c%s%c%s%c%s", DATADIR, PATHSEP, "Levels",
            PATHSEP, levelname, PATHSEP, shpname);

        fshp = fopen (fshpname, "r");
        if (fshp)
        {
          load_shapes_file(fshp, om, obj, att_body, sx, sy);
          fclose(fshp);
        }
      }
#ifdef DEBUG
      log_printf("[LOAD] Shape %d loaded", i);
#endif //DEBUG
    }
#ifdef DEBUG
  physics_dump_info(om->level->physics, obj);
#endif //DEBUG
  }
  return om;
}

//Load ObjectMap from a pre opened file
void *
load_objmap_file (FILE * fobj, char *levelname, objectmanager * om)
{
  int i, j;
  int nobj = 0;
  char bodyorcons[MAXSTR], obtname[MAXSTR], obtfile[MAXSTR], instancename[MAXSTR];
  object *obj = NULL;
  void *newom = NULL;
  void *data = NULL;
  int load_sprites, read_shapes;

  if (!om)
    return newom;

  fscanf (fobj, "%d", &nobj);
  if (nobj <= 0)
    return newom;

  for (i = 0, j = 0; i < nobj; ++i)
  {
    float ox, oy, orot;
    FILE *fobt = NULL;
    load_sprites = 0;
    read_shapes = 0;
    fscanf (fobj, "%s", bodyorcons);
    if(0 == strcmp(bodyorcons,"OBT"))
    {
      // load object instance
      fscanf (fobj, "%s%f%f%f%s", instancename, &ox, &oy, &orot, obtname);
      obj = om_create (om);
      obj->x = ox;
      obj->y = oy;
      obj->canrotate = 0;
      strncpy(obj->name,instancename,MAX_NAME_LENGTH);
      sprintf (obtfile, "%s%c%s%c%s%c%s", DATADIR, PATHSEP, "Levels",
          PATHSEP, levelname, PATHSEP, obtname);
      fobt = fopen (obtfile, "r");
      if (fobt)
      {
#ifdef DEBUG
        log_printf("[LOAD_OBJMAP] Loading object \"%s\" from object template. Now adding...",obj->name);
#endif //DEBUG
        load_object_template_file (fobt, levelname, om, obj, ox, oy, orot);
#ifdef DEBUG
        log_printf("[LOAD_OBJMAP] Loaded object \"%s\" from object template. Now adding...",obj->name);
#endif //DEBUG
        fclose(fobt);
        fobt = NULL;
      }
      om_add (om, obj, data);
    }
    else if(0 == strcmp(bodyorcons,"CBB"))
    {
      // load constraint; no object here
      char constraint[MAXSTR], oA[MAXSTR], oB[MAXSTR];
      object *objA = NULL, *objB = NULL;
      constraint_data cd;
      fscanf(fobj, "%s%s%s%d%s%d", instancename, constraint, oA, &cd.body_a, oB, &cd.body_b);
      if( -1 != cd.body_a )
        objA = om_get_object_by_name(oA);
      if( -1 != cd.body_b )
        objB = om_get_object_by_name(oB);
      if(!strcasecmp(constraint,"SLIDE"))
      {
        fscanf(fobj, "%f%f%f%f%f%f", &cd.x[0], &cd.y[0], &cd.x[1], &cd.y[1], &cd.min, &cd.max);
        physics_set_constraint_list_global (om->level->physics, j, constraint, objA, objB, &cd);
#ifdef DEBUG
        log_printf("[LOAD] Constraint %s", constraint);
#endif //DEBUG
      }
      else if(!strcasecmp(constraint,"PIVOT"))
      {
        fscanf(fobj, "%f%f", &cd.x[0], &cd.y[0]);
        physics_set_constraint_list_global (om->level->physics, j, constraint, objA, objB, &cd);
#ifdef DEBUG
        log_printf("[LOAD] Constraint %s", constraint);
#endif //DEBUG
      }
      else if(!strcasecmp(constraint,"ROTARYLIMIT"))
      {
        fscanf(fobj, "%f%f", &cd.min, &cd.max);
        physics_set_constraint_list_global (om->level->physics, j, constraint, objA, objB, &cd);
#ifdef DEBUG
        log_printf("[LOAD] Constraint %s", constraint);
#endif //DEBUG
      }
      ++j;
    }
  }
  physics_add_constraints_to_space (om->level->physics);
  return newom;
}

