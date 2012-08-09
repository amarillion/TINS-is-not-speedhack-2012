#include <header.h>
#include <gameglobal.h>
#include <log.h>
#include <util.h>
#include <shader.h>

char shader_type_id[SHADER_TYPES][MAXSTR] = { "pixel", "vertex" };
char shader_suffix[MAXSTR] = { ".shader" };

//Needed if not Fixed Function.
static shader *identity = NULL;

#define PREC ""

#ifdef IPHONE
#undef PREC
#define PREC "mediump"		//DO STUFF.
#endif //IPHONE

#ifdef IPHONE
char preproccode[MAXSTRLARGE] = {
  "#define IPHONE 1\n" "#define PREC   mediump\n" "precision PREC float;\n"
};
#else
char preproccode[MAXSTRLARGE] = {
  ""
};
#endif //IPHONE

#if (SHADER_IMPL != SHADER_NONE)
char identityshadercode[SHADER_TYPES][MAXSTRLARGE] = {
  //Pixel
  "#ifndef PREC\n"
    "#define PREC\n"
    "#endif //PREC\n"
    "uniform bool " SHADER_PARAM_USE_SRC ";\n"
    "uniform sampler2D " SHADER_PARAM_SRC ";\n"
    "uniform bool " SHADER_PARAM_USE_SRC_MATRIX ";\n"
    "uniform PREC mat4 " SHADER_PARAM_SRC_MATRIX ";\n"
    "varying PREC vec2 " SHADER_PARAM_VARYINGCOORD ";\n"
    "varying PREC vec4 " SHADER_PARAM_VARYINGCOLOR ";\n"
    "void main(){\n"
    "  PREC vec4 tmp = " SHADER_PARAM_VARYINGCOLOR ";\n"
    "  PREC vec4 coord = vec4(" SHADER_PARAM_VARYINGCOORD ", 0.0, 1.0);\n"
    "  if (" SHADER_PARAM_USE_SRC_MATRIX ") {\n"
    "     PREC vec4 sample;\n"
    "     coord = coord * " SHADER_PARAM_SRC_MATRIX ";\n"
    "     sample = texture2D(" SHADER_PARAM_SRC ", coord.st);\n"
    "     tmp *= sample;\n"
    "  } else if (" SHADER_PARAM_USE_SRC ") {\n"
    "     PREC vec4 sample;\n"
    "     sample = texture2D(" SHADER_PARAM_SRC ", coord.st);\n"
    "     tmp *= sample;\n" "  }\n" "  gl_FragColor = tmp;\n" "}\n",
  //Vertex
  "attribute vec4 pos;\n"
    "attribute vec4 color;\n"
    "attribute vec2 texcoord;\n"
    "uniform mat4 projview_matrix;\n"
    "varying vec4 " SHADER_PARAM_VARYINGCOLOR ";\n"
    "varying vec2 " SHADER_PARAM_VARYINGCOORD ";\n"
    "void main(){\n"
    "" SHADER_PARAM_VARYINGCOLOR " = color;\n"
    "" SHADER_PARAM_VARYINGCOORD " = texcoord;\n"
#ifndef LINUX
    "gl_PointSize = 1.0;\n"
#endif //LINUX
  "gl_Position = projview_matrix * pos;\n" "}\n"
};
#endif //(SHADER_IMPL != SHADER_NONE)

//Initialize Shader System.
int
init_shaders ()
{
  log_printf ("Initializing Shader System");
#if (SHADER_IMPL != SHADER_NONE)
  display_flags |= ALLEGRO_USE_PROGRAMMABLE_PIPELINE;
#endif // (SHADER_IMPL != SHADER_NONE)
#if (SHADER_IMPL == SHADER_GLSL)
  display_flags |= ALLEGRO_OPENGL;
#endif //  (SHADER_IMPL == SHADER_GLSL)
  return 0;
}

//Destroy Shader System.
void
quit_shaders ()
{
  if (identity)
    {
      destroy_shader (identity);
      identity = NULL;
    }
  log_printf ("Uninitializing Shader System");
}

//Destroy a Shader
void
destroy_shader (shader * shade)
{
  if (shade)
    {
      if (shade->shaders)
	{
#if (SHADER_IMPL != SHADER_NONE)
	  al_destroy_shader (shade->shaders);
#endif //(SHADER_IMPL != SHADER_NONE)
	  shade->shaders = NULL;
	}
      free (shade);
      shade = NULL;
#ifdef VERBOSE
      log_printf ("Destroyed Shader.");
#endif //VERBOSE
    }
}

#if (SHADER_IMPL != SHADER_NONE)
//Compile a Shader from a String.
static int
shaderhandle_compile_string (shaderhandle shade, char *string, int type)
{
  ALLEGRO_SHADER_TYPE atype;
  if (type == SHADER_PIXEL)
    {
      atype = ALLEGRO_PIXEL_SHADER;
    }
  else if (type == SHADER_VERTEX)
    {
      atype = ALLEGRO_VERTEX_SHADER;
    }
  else
    {
      return -1;
    }
  if (shade == NULL)
    {
      return -1;
    }
  {
    char finalstring[MAXSTRLARGE + MAXSTRLARGE];
    *finalstring = 0;
    sprintf (finalstring, "%s\n%s", preproccode, string);
    finalstring[MAXSTRLARGE + MAXSTRLARGE - 1] = 0;
    if (!al_attach_shader_source (shade, atype, finalstring))
      {
#ifdef VERBOSE
	log_printf
	  ("Error Compiling Shader: \n%s\n of Type: %s with Errors: \n%s\n",
	   finalstring, shader_type_id[type], al_get_shader_log (shade));
#endif //VERBOSE
	return -1;
      }
  }
  return 0;
}

//Compile a Shader from a File.
static int
shaderhandle_compile_file (shaderhandle shade, char *shader_path, int type)
{
  char *str = get_file_contents (shader_path, NULL);
  int retval = -1;
  if (str)
    {
      retval = shaderhandle_compile_string (shade, str, type);
      free (str);
      str = NULL;
    }
  return retval;
}
#endif //(SHADER_IMPL != SHADER_NONE)

//Create a Shader.
shader *
create_shader (char *shadername)
{
  shader *s = malloc (sizeof (shader));
  if (s)
    {
      int loaded;
      s->shaders = NULL;
      loaded = 0;
#if (SHADER_IMPL != SHADER_NONE)
#ifdef VERBOSE
      log_printf ("Loading Shader: %s", shadername);
#endif //VERBOSE
      s->shaders = al_create_shader (PLATFORM);
      if (s->shaders)
	{
	  char shaderfilename[MAXSTR];
	  int t;
	  for (t = 0; t < SHADER_TYPES; ++t)
	    {
	      int retval = -1;
	      if (retval == -1)
		{
		  *shaderfilename = 0;
		  sprintf (shaderfilename, "%s%c%s%c%s%c%s%c%s%c%s%s",
			   DATADIR, PATHSEP, "Shaders", PATHSEP, shadername,
			   PATHSEP, shader_type_id[t], '_', SHADER_IMPL_ID, '_',
			   PLAT_ID, shader_suffix);
		  if (file_exists (shaderfilename))
		    {
		      retval =
			shaderhandle_compile_file (s->shaders, shaderfilename,
						   t);
		    }
		}
	      if (retval == -1)
		{
		  *shaderfilename = 0;
		  sprintf (shaderfilename, "%s%c%s%c%s%c%s%c%s%s", DATADIR,
			   PATHSEP, "Shaders", PATHSEP, shadername, PATHSEP,
			   shader_type_id[t], '_', SHADER_IMPL_ID, shader_suffix);
		  if (file_exists (shaderfilename))
		    {
		      retval =
			shaderhandle_compile_file (s->shaders, shaderfilename,
						   t);
		    }
		}
	      if (retval == -1)
		{
		  retval =
		    shaderhandle_compile_string (s->shaders,
						 identityshadercode[t], t);
		}
	      if (retval != -1)
		{
		  ++loaded;
		}
	    }
	  if (loaded && !al_link_shader (s->shaders))
	    {
#ifdef VERBOSE
	      log_printf ("Error Linking Shader: %s %s", shadername,
			  al_get_shader_log (s->shaders));
#endif //VERBOSE
	      destroy_shader (s);
	      s = NULL;
	    }
	}
      if (s && s->shaders)
	{
#ifdef VERBOSE
	  log_printf ("Successfully Loaded Shader: %s", shadername);
#endif //VERBOSE
	}
      else
	{
#ifdef VERBOSE
	  log_printf ("Unable to Load Shader: %s", shadername);
#endif //VERBOSE
	  if (s)
	    {
	      destroy_shader (s);
	      s = NULL;
	    }
	}
#endif //(SHADER_IMPL != SHADER_NONE)
    }
  return s;
}

//Create an Identity Shader
shader *
identity_shader ()
{
  if (identity)
    return identity;
  identity = create_shader ("identity");
  return identity;
}
