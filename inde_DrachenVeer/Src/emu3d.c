//Source file containing emulated 3D function definitions.

#include <emu3d.h>
#include <global.h>
#include <log.h>
#include <util.h>
#include <defs.h>
#include <draw.h>

static int emu3d_mode = EMU3D_NULL;
static int emu3d_layer = 0;
static int emu3d_layers = 0;
#if (SHADER_IMPL != SHADER_NONE)
static int layer_separation = 12;
#endif //(SHADER_IMPL == SHADER_NONE)
static ALLEGRO_BITMAP *accumulation_buffer;

static shader *emu3d_shader[EMU3D_TYPES] = { NULL, NULL, NULL, NULL };

//3D Mode set
void
emu3d_render_mode (int mode)
{
#if (SHADER_IMPL == SHADER_NONE)
    mode = EMU3D_NULL;
#endif //(SHADER_IMPL == SHADER_NONE)
  if (mode >= EMU3D_NULL && mode < EMU3D_TYPES)
    {
      emu3d_mode = mode;
      emu3d_start (0);
    }
}

//3D Start
void
emu3d_start (int layers)
{
  emu3d_layer = 0;
  emu3d_layers = layers;
  if ((emu3d_mode != EMU3D_NULL))
    {
      al_set_target_bitmap (accumulation_buffer);
      al_clear_to_color (al_map_rgb (0, 0, 0));
      al_set_target_bitmap (rbuffer);
    }
}

//3D Layer Add
void
emu3d_layer_done ()
{
  if (emu3d_layer < emu3d_layers)
    {
      if (emu3d_mode != EMU3D_NULL)
	{
	  int to_render_special = 0;
	  int offset_layer = emu3d_layer;
	  int rbw = al_get_bitmap_width (rbuffer), rbh =
	    al_get_bitmap_height (rbuffer);
	  al_set_target_bitmap (accumulation_buffer);
	  if (emu3d_mode == EMU3D_ANAGLYPHIC
	      || emu3d_mode == EMU3D_TRIOCHROME)
	    {
	      offset_layer = emu3d_layer - emu3d_layers;
	    }
	  if (offset_layer)
	    to_render_special = 1;
	  if (emu3d_mode == EMU3D_FOV && (emu3d_layer != (emu3d_layers / 2)))
	    {
	      to_render_special = 1;
	    }
	  if (to_render_special)
	    {
	      set_shader (emu3d_shader[emu3d_mode]);
#if (SHADER_IMPL != SHADER_NONE)
        if (emu3d_mode == EMU3D_ANAGLYPHIC
		  || emu3d_mode == EMU3D_TRIOCHROME)
		{
		  int offset = offset_layer * layer_separation;
		  al_set_shader_float (emu3d_shader[emu3d_mode]->shaders,
				       "offset",
				       ((float) offset / (float) rbw));
		}
	      else if (emu3d_mode == EMU3D_FOV)
		{
		  float blurriness =
		    (float) ((2 * emu3d_layer) -
			     emu3d_layers) / (float) emu3d_layers;
		  if (blurriness < 0.0f)
		    blurriness = -blurriness;
		  al_set_shader_float (emu3d_shader[emu3d_mode]->shaders,
				       "blurriness", blurriness);
		}
	      else if (emu3d_mode == EMU3D_SILHOUTTE)
		al_set_shader_float (emu3d_shader[emu3d_mode]->shaders,
				     "tint",
				     ((float) (emu3d_layer) /
				      (float) emu3d_layers));
#endif //(SHADER_IMPL != SHADER_NONE)
	      blit (rbuffer, 0, 0, 0, 0, rbw, rbh, 0);
	      unset_shader ();
	    }
	  else
	    {
	      blit (rbuffer, 0, 0, 0, 0, rbw, rbh, 0);
	    }
	  al_set_target_bitmap (rbuffer);
	  al_clear_to_color (al_map_rgb (0, 0, 0));
	}
    }
  else
    {
      log_printf ("Emu-3D Layer: %d >= %d", emu3d_layer, emu3d_layers);
      return;
    }
  emu3d_layer++;
}

//3D Layer End
void
emu3d_end ()
{
  if (emu3d_mode != EMU3D_NULL)
    {
      int rbw = al_get_bitmap_width (rbuffer), rbh =
	al_get_bitmap_height (rbuffer);
      al_set_target_bitmap (rbuffer);
      blit (accumulation_buffer, 0, 0, 0, 0, rbw, rbh, 0);
    }
  emu3d_layers = 0;
}

//Uninitialize 3D Renderer
void
quit_emu3d_renderer ()
{
  int s;
  log_printf ("Quitting Emu-3D Renderer");
  if (accumulation_buffer)
    {
      al_destroy_bitmap (accumulation_buffer);
      accumulation_buffer = NULL;
    }
  for (s = 0; s < EMU3D_TYPES; ++s)
    {
      if (emu3d_shader[s])
	destroy_shader (emu3d_shader[s]);
      emu3d_shader[s] = NULL;
    }
}

//Initialize 3D Renderer
int
init_emu3d_renderer ()
{
  int s;
  log_printf ("Initializing Emu-3D Renderer");
  for (s = 0; s < EMU3D_TYPES; ++s)
    {
      emu3d_shader[s] = NULL;
    }
  emu3d_shader[EMU3D_ANAGLYPHIC] = create_shader ("anaglyphic");
  emu3d_shader[EMU3D_TRIOCHROME] = create_shader ("triochrome");
  emu3d_shader[EMU3D_FOV] = create_shader ("fov");
  emu3d_shader[EMU3D_SILHOUTTE] = create_shader ("silhoutte");
  accumulation_buffer = al_clone_bitmap (rbuffer);
  return 0;
}
