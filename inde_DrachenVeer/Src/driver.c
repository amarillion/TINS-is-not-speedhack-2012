//Source file for driver program.

#include <global.h>
#include <gui.h>
#include <log.h>
#include <fonts.h>
#include <physics.h>
#include <script.h>
#include <shader.h>
#include <state.h>
#include <statedef.h>
#include <util.h>

//Speed Counter for Timer.
volatile long speed_counter = 0;

//Flag for Initialized.
static int inited = 0;

//Loop Quit?
static int quitloop = 1;

//State
static run_state INITIAL_STATE = GAME_SPLASH;

// render in main loop
static int need_redraw = 0;

// audio parameters
#define NUM_AUDIO_SAMPLES 8

// events: logic/render/input
// timer
static ALLEGRO_TIMER *timer = NULL;
static ALLEGRO_EVENT_QUEUE *event_queue = NULL;

//Uninitializes System.
void
quit ()
{
  if (!inited)
    return;

//Cleanup Physics
  physics_quit ();

//Cleanup Scripting
  script_cleanup ();

//Unload fonts
  fm_cleanup ();

  if (icon)
    {
      al_destroy_bitmap (icon);
      icon = NULL;
    }

// unload created variables
  if (timer)
    {
      al_destroy_timer (timer);
      timer = NULL;
    }

  if (event_queue)
    {
      al_destroy_event_queue (event_queue);
      event_queue = NULL;
    }

//Cleanup Shaders.
  quit_shaders ();

  if (rbuffer)
    {
//Destroy Render Buffer
      al_destroy_bitmap (rbuffer);
      rbuffer = NULL;
      log_printf ("Destroyed Render Buffer");
    }

  if (screen)
    {
//Destroy Screen
      al_destroy_display (screen);
      screen = NULL;
      log_printf ("Destroyed Display");
    }

  if (soundsystem)
    --soundsystem;
  if (mouse)
    --mouse;
  if (touch)
    --touch;

  if (inited)
    {
      //al_uninstall_system ();
      log_printf ("Allegro UnInitialized");
      --inited;
    }

//Uninitialize Logging
  quit_log ();
}

//Initializes Screen and Render Buffer.
int
init_screen ()
{
  display_flags = 0;

  init_shaders ();

#ifdef IPHONE
  display_flags |= ALLEGRO_FULLSCREEN_WINDOW;
  al_set_new_display_option (ALLEGRO_SUPPORTED_ORIENTATIONS,
			     ALLEGRO_DISPLAY_ORIENTATION_LANDSCAPE, 1);
#else
  if (windowed_mode)
    {
      display_flags |= ALLEGRO_WINDOWED;
    }
  else
    {
      display_flags |= ALLEGRO_FULLSCREEN;
    }
#endif //IPHONE

  al_set_new_display_flags (display_flags);

  if (!screen)
    {
#ifdef IPHONE
      screen = al_create_display (0, 0);
#else
      screen = al_create_display (xres, yres);
#endif //IPHONE
    }

  if (!screen)
    {
      log_printf ("Failed to create display.");
      return -1;
    }
  xres = al_get_display_width (screen);
  yres = al_get_display_height (screen);

  if (windowed_mode)
    {
      log_printf ("Screen Initialized: %d x %d Windowed", xres, yres);
    }
  else
    {
      log_printf ("Screen Initialized: %d x %d FS", xres, yres);
    }

  rbuffer = al_clone_bitmap (al_get_backbuffer (screen));
  if (!rbuffer)
    {
      log_printf ("Failed to create Render Buffer.");
      return -1;
    }
  log_printf ("Created Render Buffer");

  return 0;
}

//Initializes System.
int
init ()
{
  int status = -1;

//Initialize Logging
  init_log ();

//Start Logging
  log_printf (PRODTITLE " - " PRODVER " Started");

//Set Information Before Allegro Initialization
  al_set_org_name (COMPANYNAME);
  al_set_app_name (PRODNAME);

//Initialize Allegro
  if (false == (status = al_init ()))
    return status;
  log_printf ("Allegro Initialized");

  //Initialize Timer
  timer = al_create_timer (1.0 / lps);
  if (!timer)
    {
      log_printf ("Failed to create timer");
      return -1;
    }
  log_printf ("Timer Initialized");

//Initialize Random Number Generator
  srand (time (NULL));
  log_printf ("Initialized Random Number Seed");

  //Queues for Events.
  event_queue = al_create_event_queue ();
  if (NULL == event_queue)
    {
      log_printf ("Failed to create event queue");
      return -1;
    }

// Initialize Images
  if (false == (status = al_init_image_addon ()))
    {
      log_printf ("Image Addon disabled");
      return -1;
    }
  else
    {
      log_printf ("Image Addon Initialized");
    }

//Initialize Font system
  if (fm_init () != 0)
    {
      log_printf ("Font system disabled");
      return -1;
    }

//Load Default Font
  log_printf ("Font system Initialized");

//Initialize Screen
  if (-1 == (status = init_screen ()))
    return status;

//Register Default Events. 
  al_register_event_source (event_queue,
			    al_get_display_event_source (screen));
  al_register_event_source (event_queue, al_get_timer_event_source (timer));

  //Set Screen Blank.
  al_set_target_backbuffer (screen);
  al_clear_to_color (al_map_rgb (0, 0, 0));

//Initialize Sound System
// note: in A5, we have an explicit choice between hw(voice) and sw(mixer)
// We let Allegro decide which to use
  if (false == (status = al_install_audio ()))
    {
      soundsystem = 0;
      log_printf ("Sound system disabled");
    }
  else
    {
      log_printf ("Sound system Initialized");
      ++soundsystem;
    }
  if (false == (status = al_init_acodec_addon ()))
    {
      soundsystem = 0;
      log_printf ("Audio Addon disabled. Sound system disabled.");
    }
  else
    {
      log_printf ("Audio Addon Initialized");
      ++soundsystem;
    }
  if (soundsystem)
    {
//set up a default mixer/voice
//      al_reserve_samples (NUM_AUDIO_SAMPLES);
    }

// TODO: A5 audio addon details missing from online tutorials
// TODO: The alternative is to use our own voice/mixer combo like the following.
// It allows us to customize audio parameters.
// ALLEGRO_VOICE *voice;
// ALLEGRO_MIXER *mixer;
// voice = al_create_voice(44100, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_2);
// test for voice OK
// mixer = al_create_mixer(44100, ALLEGRO_AUDIO_DEPTH_FLOAT32, ALLEGRO_CHANNEL_CONF_2);
// test for mixer OK
// status = al_attach_mixer_to_voice(mixer, voice);
// test for status OK
// audio stream events like so:
// al_register_event_source(event_queue, al_get_audio_stream_event_source(ALLEGRO_AUDIO_STREAM *stream));
// sound stream binding like so:
// al_attach_audio_stream_to_mixer(stream, al_get_default_mixer());

//Initialize Primitives
  if (false == (status = al_init_primitives_addon ()))
    {
      log_printf ("Primitives disabled");
      return -1;
    }
  log_printf ("Primitives Enabled");

//Initialize Keyboard
  if (false == (status = al_install_keyboard ()))
    return status;
  log_printf ("Keyboard Initialized");

//Initialize Mouse
  if (false == (status = al_install_mouse ()))
    {
      mouse = 0;
      log_printf ("Mouse disabled");
    }
  else
    {
      log_printf ("Mouse Initialized");
      ++mouse;
      // Display mouse cursor
      al_show_mouse_cursor (screen);
    }

//Initialize Mouse
  if (false == (status = al_install_touch_input ()))
    {
      touch = 0;
      log_printf ("Touch disabled");
    }
  else
    {
      log_printf ("Touch Initialized");
      ++touch;
    }


  al_register_event_source (event_queue, al_get_keyboard_event_source ());
  if (mouse)
    al_register_event_source (event_queue, al_get_mouse_event_source ());
  if (touch)
    al_register_event_source (event_queue,
			      al_get_touch_input_event_source ());

  gui_register_source (event_queue);

//Initialize Window Title and Other Parameters.
  al_set_window_title (screen, PRODTITLE);
  {
    char iconfilename[MAXSTR];
    *iconfilename = 0;
    sprintf (iconfilename, "%s%c%s", DATADIR, PATHSEP, ICON);
    if (file_exists (iconfilename))
      {
	icon = load_bitmap_ex (iconfilename, NULL);
	if (icon)
	  al_set_display_icon (screen, icon);
      }
  }

//Initialize Scripting
  if (-1 == (status = script_init ()))
    return status;

  //Initialize Physics
  if (-1 == (status = physics_init ()))
    return status;

  ++inited;
  return 0;
}

//Main Loop Quit
void
loop_quit ()
{
  log_printf ("Quitting Loop");
}

int
loop_init ()
{
  int status = -1;

//Set Timer
  al_start_timer (timer);

  log_printf ("Creating Main Loop at %d LPS", lps);
  quitloop = 0;

//Switch to State.
  log_printf ("Switching to Initial State");
  state_switch (INITIAL_STATE);

  status = 0;
  return status;
}

//Game Loop Init
void
loop_run ()
{
  log_printf ("Starting Screen ....");
  state_logic ();
  state_render ();

  while (!quitloop)
    {
      do
	{
	  ALLEGRO_EVENT ev;
	  al_wait_for_event (event_queue, &ev);
	  switch (ev.type)
	    {
	    case ALLEGRO_EVENT_TIMER:
	      ++speed_counter;
	      need_redraw = 1;
	      break;
	    case ALLEGRO_EVENT_DISPLAY_CLOSE:
	      terminate_state_loop ();
	      break;
#ifdef IPHONE
	    case ALLEGRO_EVENT_DISPLAY_HALT_DRAWING:
	    case ALLEGRO_EVENT_DISPLAY_SWITCH_OUT:
	      al_acknowledge_drawing_halt (screen);
	      need_redraw = 0;
	      break;
	    case ALLEGRO_EVENT_DISPLAY_SWITCH_IN:
	    case ALLEGRO_EVENT_DISPLAY_RESUME_DRAWING:
	      need_redraw = 1;
	      break;
#endif //IPHONE
	    default:
	      //Pass through to Input.
	      state_input (&ev);
	      break;
	    }
	}
      while (!al_is_event_queue_empty (event_queue));
      while (speed_counter > 0)
	{
	  --speed_counter;
	  state_logic ();
	}
      if (need_redraw)
	{
	  state_render ();
	  need_redraw = 0;
	}
    }
}

//Game Shutdown
void
game_shutdown ()
{
  quitloop = 1;
}

//Main
int
main ()
{
  if (init () == 0)
    {
      if (loop_init () == 0)
	{
	  loop_run ();
	}
      loop_quit ();
    }
  quit ();
  return 0;
}
