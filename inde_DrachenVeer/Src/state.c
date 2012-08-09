//Source file for Game States.

#include <global.h>
#include <state.h>
#include <statedef.h>
#include <draw.h>
#include <transfx.h>
#include <gui.h>

//Variables
static int state_terminated = 0, state_switched = 0, state_to_render =
  1, state_to_switch_render = 0, state_to_force_render = 0;
run_state state = GAME_UNINITIALIZED, oldstate = GAME_UNINITIALIZED;
static float prev_time, dt;

//Transition Effects.
static int state_trans_curf = -1, state_trans_totf = -1;
static ALLEGRO_BITMAP *rbufferorig = NULL, *transrbuffer = NULL;
static transfx_function strans;

//Handler
typedef struct
{
  int (*proc_init) ();
  void (*proc_logic) ();
  void (*proc_input) (ALLEGRO_EVENT * event);
  void (*proc_render) ();
  void (*proc_quit) ();
} state_handler;

static state_handler handlers[NUM_STATES] = {
  {usual_init, usual_logic, usual_input, usual_render, usual_quit},
  {game_play_init, game_play_logic, game_play_input, game_play_render,
   game_play_quit},
  {splash_init, splash_logic, splash_input, splash_render, splash_quit}
};

//Transition Render Stop for States
void
trans_stop ()
{
  if (rbufferorig)
    al_destroy_bitmap (rbufferorig);
  rbufferorig = NULL;
  if (transrbuffer)
    al_destroy_bitmap (transrbuffer);
  transrbuffer = NULL;
  strans = NULL;
  state_trans_curf = -1;
  state_trans_totf = -1;
}

//Transition Render Start for States
void
trans_start (transfx_function trans, int totf)
{
  if (totf <= 0)
    return;
  rbufferorig = NULL;
  transrbuffer = NULL;
  strans = trans;
  state_trans_curf = 0;
  state_trans_totf = totf;
}

//Logic for States
void
state_logic ()
{
  if (state_terminated)
    {
      if (state_terminated > 0)
	{
	  trans_stop ();
	  state_quit ();
	  game_shutdown ();
	  state_terminated = -1;	//One Time Quit.
	}
      return;
    }

  if (state_switched)
    {
      return;
    }

  handlers[state].proc_logic ();
  {
    dt = al_get_time () - prev_time;
    while (dt >= GUI_DT)
      {
	gui_update ();
	dt -= GUI_DT;
	prev_time = al_get_time ();
      }
    gui_update ();
  }
}

//Input for States
void
state_input (ALLEGRO_EVENT * event)
{
// check special cases
  if (state_switched)
    return;
  if (state_terminated)
    return;

// process input
  handlers[state].proc_input (event);

  // give to GUI.
  gui_input (event);
}

//Skip Render for States
void
skip_render ()
{
  state_to_render = 0;
}

//Render for States
void
state_render ()
{
  if (state_switched)
    return;
  if (state_terminated)
    return;

  state_to_render = 1;
  state_to_force_render = 0;

//To Set up for Transition Render
  if (state_trans_curf >= 0)
    {
      if (rbufferorig == NULL)
	rbufferorig = al_clone_bitmap (rbuffer);
      if (transrbuffer == NULL)
	transrbuffer = al_clone_bitmap (rbuffer);
    }

//Common for All Games
  al_set_target_bitmap (rbuffer);
  handlers[state].proc_render ();

//Transition Render
  if (state_trans_curf >= 0 && state_trans_curf < state_trans_totf && strans
      && rbufferorig && transrbuffer)
    {
      strans (rbufferorig, rbuffer, transrbuffer, state_trans_curf++,
	      state_trans_totf);
      state_to_force_render = 1;
    }

//To Destroy Transition
  if (strans && (state_trans_curf >= state_trans_totf))
    {
      trans_stop ();
      state_to_force_render = 1;
    }

//Switch Modes
  if (state_to_switch_render)
    init_screen ();

//This is Because of GUI.    
  state_to_force_render = 1;

  if (state_to_force_render || state_to_switch_render || state_to_render)
    {
//Render to Screen
      al_set_target_backbuffer (screen);
      if (transrbuffer)
	blit (transrbuffer, 0, 0, 0, 0, xres, yres, 0);
      else
	blit (rbuffer, 0, 0, 0, 0, xres, yres, 0);
      gui_render ();
#ifdef LINUX
//VSync helped remove bad artifacts in Linux
      al_wait_for_vsync ();
#endif //LINUX
      al_flip_display ();
    }

  state_to_switch_render = 0;
}

//Quit for States
void
state_quit ()
{
  gui_quit ();
  handlers[state].proc_quit ();
  unset_shader ();
}

//Init for States
int
state_init ()
{
  prev_time = al_get_time ();
  set_shader (identity_shader ());
  gui_init ();
  return handlers[state].proc_init ();
}

//Quit for States
void
state_switch (run_state newstate)
{
  if (state != GAME_UNINITIALIZED)
    {
      state_quit ();
    }

  oldstate = state;
  state = newstate;

  ++state_switched;

  if (state_init ())
    {				//Not for GAME_USUAL
      state_quit ();
      state = GAME_USUAL;
      (void) state_init ();
    }
  --state_switched;
}

//Quit State Loop
void
terminate_state_loop ()
{
  ++state_switched;
  ++state_terminated;
}
