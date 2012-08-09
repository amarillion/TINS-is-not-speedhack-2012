//Source file for Usual Game States.

#include <global.h>
#include <state.h>
#include <log.h>
#include <util.h>

static int usual_needs_redraw = 0;

//Usual Init
int
usual_init ()
{
  usual_needs_redraw = 1;
  return 0;
}

//Usual Logic
void
usual_logic ()
{
}

//Usual Input
void
usual_input (ALLEGRO_EVENT * event)
{
  int code;
  switch (event->type)
    {
    case ALLEGRO_EVENT_KEY_DOWN:
    case ALLEGRO_EVENT_KEY_CHAR:
      code = event->keyboard.keycode;
      break;
    default:
      return;
    }
  if (event->type != ALLEGRO_EVENT_KEY_DOWN)
    return;
  if (code == ALLEGRO_KEY_ESCAPE)
    {
      terminate_state_loop ();
      return;
    }
  if (code == ALLEGRO_KEY_ENTER)
    {
      state_switch (oldstate);
      return;
    }
}

//Usual Render
void
usual_render ()
{
  if (usual_needs_redraw)
    {
      al_set_target_bitmap (rbuffer);
      al_clear_to_color (al_map_rgb (255, 0, 0));
      usual_needs_redraw = 0;
    }
  else
    {
      skip_render ();
    }
}

//Usual Quit
void
usual_quit ()
{
}
