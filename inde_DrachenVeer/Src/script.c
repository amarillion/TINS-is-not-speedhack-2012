#include <script.h>
#include <log.h>
#include <object.h>
#include <util.h>
#include <list.h>

//Definitions
#define OBJ_TABLE "objecttable"

//Declarations
//void script_lookup_handlers(level *lev);

//Variables
lua_State *lua = NULL;
static objectmanager *_om = NULL;
static level *_level = NULL;

#define MAX_CUSTOM_HANDLERS 256
static int num_handlers_;
typedef struct custom_handler_ {
	int handler_type;
	const char* func_name;
	char *obj_name;
	char *obj_pair_name;
} custom_handler;
custom_handler custom_handlers[MAX_CUSTOM_HANDLERS];

//lua_pushlightuserdata (lua, (void *) obj);
#define DISPATCH_DEFINE(NAME, PREFIX, HANDY) \
        void * \
        script_ ## NAME ## _dispatch_ ## HANDY (object * obj, void *data) \
        { \
          char funcname[MAXSTR]; \
          void *ret; \
          sprintf (funcname, "%s_%s", #HANDY, PREFIX); \
          lua_getglobal (lua, funcname); \
          lua_pushstring (lua, obj->name); \
          lua_pushlightuserdata (lua, (void *) data); \
          if (0 != lua_pcall (lua, 2, 1, 0)) { \
            log_printf \
              ("[SCRIPT] Could not call Lua callback function %s for object \"%s\": %s", \
               funcname, obj->name, lua_tostring (lua, -1)); \
            luaL_error (lua, \
                "[SCRIPT] Could not call Lua callback function %s for object \"%s\": %s", \
                funcname, obj->name, lua_tostring (lua, -1)); \
            return NULL; \
          } \
          ret = (void *) lua_touserdata (lua, -1); \
          lua_pop (lua, 1); \
          return ret; \
        }

DISPATCH_DEFINE(name,obj->name,init);
DISPATCH_DEFINE(name,obj->name,destroy);
DISPATCH_DEFINE(name,obj->name,update);
DISPATCH_DEFINE(name,obj->name,landed);

objectmanager *
get_scriptload_objectmanager ()
{
  return _om;
}

void
script_add_to_dictionary (lua_State * L, char *key, void *value)
{
// retrieve global table onto stack
  lua_getglobal (L, OBJ_TABLE);
  if (!lua_istable (L, -1))
    {
      luaL_error (L, "[SCRIPT] `%s' is not a valid Lua table", OBJ_TABLE);
      log_printf ("[SCRIPT]`%s' is not a valid Lua table", OBJ_TABLE);
      return;
    }
//key: objectid (string)
  lua_pushstring (L, key);
//value: object pointer (void *)
  lua_pushlightuserdata (L, value);
//T[key] = value;
  lua_settable (L, -3);
}

void
script_remove_from_dictionary (char *key)
{
	lua_State * L = lua;
// retrieve global table onto stack
  lua_getglobal (L, OBJ_TABLE);
  if (!lua_istable (L, -1))
    {
      luaL_error (L, "[SCRIPT] `%s' is not a valid Lua table", OBJ_TABLE);
      log_printf ("[SCRIPT]`%s' is not a valid Lua table", OBJ_TABLE);
      return;
    }
//key: objectid (string)
  lua_pushstring (L, key);
//value: nil
  lua_pushnil(L);
//T[key] = nil;
  lua_settable (L, -3);
}

int
script_init ()
{
  lua = lua_open ();
  if (NULL == lua)
    {
      log_printf ("[SCRIPT] Unable to open Lua interpreter.");
      return -1;
    }
  luaL_openlibs (lua);
  log_printf ("[SCRIPT] Lua and libraries successfully loaded");
// register library with Lua
//luaL_openlib(lua, "object", object_members, 0);
  script_reset ();
//call SWIG wrapper that registers our C functions to Lua
  luaopen_game (lua);
  return 0;
}

int
l_FindObjectByName (lua_State * L)
{
  const char *key = luaL_checkstring (L, -1);
  lua_pop (L, -1);
//dictionary lookup
  lua_getglobal (L, OBJ_TABLE);	//T
  if (!lua_istable (L, -1))
    {
      luaL_error (L, "[SCRIPT] `%s' is not a valid Lua table", OBJ_TABLE);
      log_printf ("[SCRIPT] `%s' is not a valid Lua table", OBJ_TABLE);
      return 0;
    }
  lua_pushstring (L, key);	//key
  lua_gettable (L, -2);		//stack <--- T[key]
// if lookup failed, return NULL
  if (!lua_isuserdata (L, -1))
    {
      lua_pop (L, 1);
      lua_pushlightuserdata (L, NULL);
    }
//stack: [void *]
  return 1;
}

void
script_cleanup ()
{
  if (lua)
    {
      lua_close (lua);
      lua = NULL;
    }
}

void
script_reset ()
{
  if (NULL == lua)
    return;
// send current table to the Lua garbage collector
  lua_pushnil (lua);
  lua_setglobal (lua, OBJ_TABLE);
  lua_pop (lua, -1);
//create global lookup table (key: objectid, value: void*)
  lua_newtable (lua);
  lua_setglobal (lua, OBJ_TABLE);
  lua_pop (lua, -1);
  num_handlers_ = 0;
  memset(custom_handlers,0,MAX_CUSTOM_HANDLERS*sizeof(custom_handler));
}

static int script_execute_line (const char *line)
{
  int status = 0;
#ifdef VERBOSE
  log_printf ("[SCRIPT] Executing Line", line);
#endif //VERBOSE
  status = luaL_dostring (lua, line);
  if(0 != status)
  {
    const char *err_msg = lua_tostring(lua, -1);
    log_printf("[SCRIPT] ERROR: \"%s\"", err_msg);
  }
  return status;
}

void *
script_name_dispatch_interaction (object *a, object *b, void *data)
{
  char funcname[MAXSTR];
  void *ret;
  sprintf (funcname, "%s_%s_%s", OBJECT_HANDLER_NAMES[OBJECT_HANDLER_INTERACT], a->name, b->name);
  lua_getglobal (lua, funcname);
  lua_pushstring (lua, a->name);
  lua_pushstring (lua, b->name);
  lua_pushlightuserdata (lua, (void *) data);
  if (0 != lua_pcall (lua, 3, 1, 0)) {
    log_printf ("Could not call Lua function %s: %s", funcname, lua_tostring (lua, -1));
    luaL_error (lua, "Could not call Lua function %s: %s", funcname, lua_tostring (lua, -1));
    return NULL;
  }
  ret = (void *) lua_touserdata (lua, -1);
  lua_pop (lua, 1);
  return ret;
}

static
object_handler_func_ptr script_name_dispatchers[OBJECT_HANDLER_TYPES] = {
		script_name_dispatch_init,
		script_name_dispatch_destroy,
		script_name_dispatch_update,
		NULL
};

/**
 * This function should be called after the Lua globals have been processed.
 * It assigns the scanned custom handlers to the corresponding objects.
 */
static void
script_assign_custom_handlers(lua_State *L)
{
	int h;
	object *found, *found_pair;
	char firstname[MAXSTR];
	char *suffix, *ext;
	for(h=0; h<num_handlers_; ++h) {
		switch(custom_handlers[h].handler_type) {
		case OBJECT_HANDLER_INTERACT:
			suffix=(char*)custom_handlers[h].obj_name;
			ext = &firstname[0];
			for(;*suffix!='\0' && *suffix!='_';++suffix,++ext) {
				*ext = *suffix;
			}
			*ext = '\0';
			if(*suffix!='_')
				break;
			++suffix;
			custom_handlers[h].obj_pair_name = suffix;
			found = om_get_object_by_name(firstname);
			found_pair = om_get_object_by_name(custom_handlers[h].obj_pair_name);
			if(found && found_pair) {
				om_set_interaction(found, found_pair, script_name_dispatch_interaction);
				log_printf("        Assigned custom handler \"%s\" from script.", custom_handlers[h].func_name);
			}
			break;
		default:
			found = om_get_object_by_name(custom_handlers[h].obj_name);
			if(found)
			{
			  int f = custom_handlers[h].handler_type;
			  found->handlers._func[f] = script_name_dispatchers[f];
			  log_printf("        Assigned custom handler \"%s\" from script.", custom_handlers[h].func_name);
			}
			break;
		}
	}
}

/**
 * This function scans through the Lua global table _G
 * and stores "HANDLER_" prefixes that the user has defined
 * (where HANDLER is one of init/update/etc.)
 */
static void
script_lookup_custom_handlers(lua_State *L)
{
	int f, len;
        log_printf("%s","[SCRIPT]: GLOBALS _G from Lua:");
	//stack: -1 => ?
	lua_getglobal (lua, "_G");	//stack: -1 => _G,  -2 => ?
	if (!lua_istable (lua, -1))
	{
		luaL_error (lua, "[SCRIPT] `%s' is not a valid Lua table", "_G");
		log_printf ("[SCRIPT] `%s' is not a valid Lua table", "_G");
		return;
	}
    // Push another reference to the table on top of the stack (so we know
    // where it is, and this function can work for negative, positive and
    // pseudo indices
    lua_pushvalue(L, -1);
    // stack now contains: -1 => table
    lua_pushnil(L);
    // stack now contains: -1 => nil; -2 => table
    while (lua_next(L, -2))
    {
        const char *key;
        const char *value;

        // stack now contains: -1 => value; -2 => key; -3 => table
        // copy the key so that lua_tostring does not modify the original
        lua_pushvalue(L, -2);
        // stack now contains: -1 => key; -2 => value; -3 => key; -4 => table
        key = lua_tostring(L, -1);
        value = lua_tostring(L, -2);
        log_printf("    VAR: %s => %s",key,value);
        for(f=0; f<OBJECT_HANDLER_TYPES; ++f) {
        	const char *handler_name = OBJECT_HANDLER_NAMES[f];
        	len = strlen(handler_name);
		if(strncasecmp(key,handler_name,len) == 0) {
				//Expected format: {init|update|destroy}_objname
				char *objname = (char *)key + len + 1;
				if(num_handlers_ < MAX_CUSTOM_HANDLERS) {
					custom_handlers[num_handlers_].func_name = key;
					custom_handlers[num_handlers_].obj_name = objname;
					custom_handlers[num_handlers_].handler_type = f;
					log_printf("    [%s] Custom %s function defined for object \"%s\"", custom_handlers[num_handlers_].func_name, handler_name, objname);
					++num_handlers_;
				}
				else
				{
					log_printf("    [WARN] Insufficient slots for custom handlers.");
				}
		}
    	}
        // pop value + copy of key, leaving original key
        lua_pop(L, 2);
        // stack now contains: -1 => key; -2 => table
    }
    // stack now contains: -1 => table (when lua_next returns 0 it pops the key
    // but does not push anything.)
    // Pop table
    lua_pop(L, 1);
    script_assign_custom_handlers(L);
    lua_pop (lua, -1);
    // Stack is now the same as it was on entry to this function
}

void
script_execute_file (const char *path, level * level)
{
  int status = 0;
#ifdef VERBOSE
  log_printf ("[SCRIPT] Executing File: \"%s\"", path);
#endif //VERBOSE
//Required because script interacts with objectmanager, e.g. object creation
  _level = level;
  _om = level->objectmanager;
//loads file and executes it
  {
    char *str = get_file_contents ((char *) path, NULL);
    if (str)
      {
	status = script_execute_line (str);
	free (str);
	str = NULL;
      }
  }
  if(status == 0)
  {
    script_execute_init();
    script_lookup_custom_handlers(lua);
    log_printf ("[SCRIPT] Executed File: \"%s\"", path);
  }
  _om = NULL;
  _level = NULL;
}


static int
script_call_function (const char *funcname, const char *arg)
{
  int retval = -1, nargs = 0;
  lua_getglobal (lua, funcname);
  if (NULL != arg)
    {
      lua_pushstring (lua, arg);
      nargs = 1;
    }
  retval = lua_pcall (lua, nargs, 0, 0);
  if (retval != 0)
    {
      const char *errstr = luaL_checkstring (lua, -1);
      lua_pop (lua, -1);
      luaL_error (lua, "[SCRIPT] Error calling function \"%s\": %s",
       funcname, errstr);
      log_printf ("[SCRIPT] Error calling function `%s': %s",
       funcname, errstr);
      retval = -1;
    }
  else
    {
      retval = 0;
    }
  return retval;
}

void
script_execute_init()
{
  script_call_function("script_init", NULL);
}

int
script_update_object (object * objptr)
{
  char expected_function_name[MAXSTR];
  int retval = -1;
//Try object-name-specific update
  sprintf (expected_function_name, "update_%s", objptr->name);
  retval = script_call_function (expected_function_name, NULL);
  #ifdef DEBUG
	log_printf("[SCRIPT] Trying to call: %s", expected_function_name);
  #endif //DEBUG
  if (-1 != retval)
    return 0;
//Try object-type-wide update
  sprintf (expected_function_name, "update_%s",
	   object_typename_table[objptr->type]);
  retval = script_call_function (expected_function_name, objptr->name);
  if (-1 != retval)
    return 0;
  return retval;
}
