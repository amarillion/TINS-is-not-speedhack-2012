#ifndef _SHADER_H_
#define _SHADER_H_

//Shader Platform Definitions.
#define SHADER_NONE 0
#define SHADER_GLSL 1
#define SHADER_CG   2
#define SHADER_HLSL 3

#define SHADER_IMPL SHADER_NONE

#if (SHADER_IMPL != NONE) && (SHADER_IMPL != SHADER_GLSL)
#error Unsupported in Allegro 5.1
#endif // (SHADER_IMPL != NONE) && (SHADER_IMPL != SHADER_GLSL)

#if (SHADER_IMPL == SHADER_NONE)
#define SHADER_IMPL_ID "none"
#endif //(SHADER_ NONE

#if (SHADER_IMPL == SHADER_GLSL)
#include "allegro5/allegro_opengl.h"
#include "allegro5/allegro_shader_glsl.h"
#define SHADER_IMPL_ID "glsl"
#define TOP 1
#define BOT 0
#define PLATFORM ALLEGRO_SHADER_GLSL
#endif //(SHADER_IMPL == SHADER_GLSL)

//Shader Definitions.
#define SHADER_PIXEL 0
#define SHADER_VERTEX 1
#define SHADER_FRAGMENT SHADER_PIXEL
#define SHADER_TYPES  2

//What's Set by Us.
#define SHADER_PARAM_AUX "aux"
#define SHADER_PARAM_COLOR "tint"

//What's Set by Us Automatically.
#define SHADER_PARAM_DEST "dest"
#define SHADER_PARAM_TIME "time"

//What's Usually Set by Allegro.
#define SHADER_PARAM_SRC "tex"
#define SHADER_PARAM_USE_SRC "use_tex"
#define SHADER_PARAM_SRC_MATRIX "tex_matrix"
#define SHADER_PARAM_USE_SRC_MATRIX "use_tex_matrix"
#define SHADER_PARAM_VARYINGCOLOR "varying_color"
#define SHADER_PARAM_VARYINGCOORD "varying_texcoord"

#if (SHADER_IMPL == SHADER_NONE)
typedef void *shaderhandle;
#else
typedef ALLEGRO_SHADER *shaderhandle;
#endif //(SHADER_IMPL == SHADER_NONE)

typedef struct _shader
{
  shaderhandle shaders;
} shader;

//Initialize Shader System.
int init_shaders ();

//Destroy Shader System.
void quit_shaders ();

//Destroy a Shader
void destroy_shader (shader * shade);

//Create an Identity Shader
shader *identity_shader ();

//Create a Shader
shader *create_shader (char *shadername);

#endif //_SHADER_H_
