//Source file for miscellaneous functions and utilities.

#include <draw.h>
#include <global.h>
#include <log.h>

#if (SHADER_IMPL != SHADER_NONE)
//Variables.
#define SHADER_STACK_SIZE 64
static shader *shader_stack[SHADER_STACK_SIZE] = { NULL, };

static int shader_stack_top = -1;
#endif //(SHADER_IMPL != SHADER_NONE)

#if (SHADER_IMPL == SHADER_NONE)
//Sets Current Shader for Draw.
void
set_shader (shader * s)
{
}

//Unsets Current Shader for Draw.
void
unset_shader ()
{
}

//Sets Shader Parameters for Current Shader for Draw.
#define set_shader_params(...)

#else

//Peeks Shader from Stack
static shader *
peek_shader ()
{
  if (shader_stack_top < 0)
    {
#ifdef VERBOSE
      log_printf ("Peek: Shader Stack is Empty.");
#endif //VERBOSE
      return NULL;
    }
  return shader_stack[shader_stack_top];
}

//Pushes Shader to Stack
static void
push_shader (shader * shade)
{
  int new_top = shader_stack_top + 1;
  if (new_top >= SHADER_STACK_SIZE)
    {
#ifdef VERBOSE
      log_printf ("Push: Shader Stack Full.");
#endif //VERBOSE
    }
  else
    {
      shader_stack[new_top] = shade;
      shader_stack_top = new_top;
    }
}

//Pops Shader from Stack
static void
pop_shader ()
{
  int new_top = shader_stack_top - 1;
  if (shader_stack_top < 0)
    {
#ifdef VERBOSE
      log_printf ("Pop: Shader Stack is Empty.");
#endif //VERBOSE
    }
  else
    {
      shader_stack[shader_stack_top] = NULL;
      shader_stack_top = new_top;
    }
}

//Sets Shader Parameters for Current Shader for Draw.
static void
set_shader_params (ALLEGRO_BITMAP * aux, ALLEGRO_COLOR * color)
{
  shader *current = peek_shader ();
  if (current)
    {
#if (SHADER_IMPL == SHADER_GLSL)
      if (screen)
	al_set_opengl_program_object (screen,
				      al_get_opengl_program_object
				      (current->shaders));
#endif //(SHADER_IMPL == SHADER_GLSL)
      if (color)
	{
	  float fcol[4];
	  int r = color->r, g = color->g, b = color->b, a = color->a;
	  if (a < 0)
	    a = 255;
	  fcol[0] = r / 255.0;
	  fcol[1] = g / 255.0;
	  fcol[2] = b / 255.0;
	  fcol[3] = a / 255.0;
	  al_set_shader_float_vector (current->shaders, SHADER_PARAM_COLOR, 4,
				      fcol, 1);
	}
      if (aux)
	al_set_shader_sampler (current->shaders, SHADER_PARAM_AUX, aux, 2);
      {
	float time = (float) al_get_time ();
	al_set_shader_float (current->shaders, SHADER_PARAM_TIME, time);
      }
      al_set_shader_sampler (current->shaders, SHADER_PARAM_DEST,
			     al_get_target_bitmap (), 1);
      al_use_shader (current->shaders, true);
    }
}

//Sets Current Shader for Draw.
void
set_shader (shader * s)
{
  push_shader (s);
  set_shader_params (NULL, NULL);
}

//Unsets Current Shader for Draw.
void
unset_shader ()
{
  shader *current = peek_shader ();
  if (current)
    {
      pop_shader ();
      al_use_shader (current->shaders, false);
      set_shader_params (NULL, NULL);
    }
}

#endif //(SHADER_IMPL == SHADER_NONE)

//Blits a Tinted Bitmap to Screen
void
tinted_blit (ALLEGRO_BITMAP * src, ALLEGRO_BITMAP * aux,
	     ALLEGRO_COLOR * color, float sx, float sy, float dx, float dy,
	     float sw, float sh, int flags)
{
  set_shader_params (aux, color);
  al_draw_bitmap_region (src, sx, sy, sw, sh, dx, dy, flags);
}

//Blits a Bitmap to Screen
void
blit (ALLEGRO_BITMAP * src, float sx, float sy, float dx, float dy, float sw,
      float sh, int flags)
{
  set_shader_params (NULL, NULL);
  al_draw_bitmap_region (src, sx, sy, sw, sh, dx, dy, flags);
}

//Stretches a Bitmap and Draws to Screen
void
stretch_blit (ALLEGRO_BITMAP * src, float sx, float sy, float sw, float sh,
	      float dx, float dy, float dw, float dh, int flags)
{
  set_shader_params (NULL, NULL);
  al_draw_scaled_bitmap (src, sx, sy, sw, sh, dx, dy, dw, dh, flags);
}

//Rotates a Bitmap and Blits to Screen
void
rotated_blit (ALLEGRO_BITMAP * src, float sx, float sy, float dx, float dy,
	      float sw, float sh, float angle, int flags)
{
 if(angle != 0.0f) {
  ALLEGRO_TRANSFORM old, T;
  al_copy_transform (&old, al_get_current_transform ());
  al_build_transform (&T, dx, dy, 1.0f, 1.0f, angle);
  al_use_transform (&T);
  set_shader_params (NULL, NULL);
  al_draw_bitmap_region (src, sx, sy, sw, sh, sw / -2.0f, sh / -2.0f, flags);
  al_use_transform (&old);
 } else {
  set_shader_params (NULL, NULL);
  al_draw_bitmap_region (src, sx, sy, sw, sh, dx - (sw/2.0f), dy - (sh/2.0f), flags);
 }
}
