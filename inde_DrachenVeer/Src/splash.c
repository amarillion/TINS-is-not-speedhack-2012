//Source file for Splash Game States.

#include <global.h>
#include <state.h>
#include <log.h>
#include <util.h>
#include <sprite.h>
#include <gameglobal.h>
#include <draw.h>

static int splash_needs_redraw = 0;
static ALLEGRO_BITMAP *title = NULL;
static ALLEGRO_BITMAP *instructions = NULL;

#define TITLE "Title.png"
#define INSTRUCTIONS "Instructions.png"

//splash Init
int
splash_init ()
{
    char filename[MAXSTR];
    *filename = 0;
    sprintf (filename, "%s%c%s", DATADIR, PATHSEP, TITLE);
    if (file_exists (filename))
      {
        title = load_bitmap_ex (filename, NULL);
      }
    *filename = 0;
    sprintf (filename, "%s%c%s", DATADIR, PATHSEP, INSTRUCTIONS);
    if (file_exists (filename))
      {
        instructions = load_bitmap_ex (filename, NULL);
      }
  splash_needs_redraw = 1;
  return 0;
}

//splash Logic
void
splash_logic ()
{
      if(!title)
      state_switch (GAME_PLAY);
}

//splash Input
void
splash_input (ALLEGRO_EVENT * event)
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
  if (code == ALLEGRO_KEY_SPACE)
    {
      state_switch (GAME_PLAY);
      return;
    }
}

//splash Render
void
splash_render ()
{
  if (splash_needs_redraw)
    {
      al_set_target_bitmap (rbuffer);
      al_clear_to_color (al_map_rgb (241, 239, 241));
      if(title)
    	  blit(title,0,0,80,100,al_get_bitmap_width(title),al_get_bitmap_height(title),0);
      if(instructions)
    	  blit(instructions,0,0,160,300,al_get_bitmap_width(instructions),al_get_bitmap_height(instructions),0);
      splash_needs_redraw = 0;
    }
  else
    {
      skip_render ();
    }
}

//splash Quit
void
splash_quit ()
{
	if(title)
		al_destroy_bitmap(title);
	title = NULL;
	if(instructions)
		al_destroy_bitmap(instructions);
	instructions =NULL;
}
