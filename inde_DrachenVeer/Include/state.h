#ifndef _STATE_H_
#define _STATE_H_

//Header file for States.

#include <header.h>

//Loop States
typedef enum
{
  GAME_UNINITIALIZED = -1,
  GAME_USUAL = 0,
  GAME_PLAY,
  GAME_SPLASH,
  NUM_STATES
} run_state;

extern run_state oldstate, state;

//Init for States
int state_init ();

//Logic for States
void state_logic ();

//Input for States
void state_input (ALLEGRO_EVENT * event);

//Skip Render for States
void skip_render ();

//Render for States
void state_render ();

//Switch to State
void state_switch (run_state newstate);

//Quit for States
void state_quit ();

//Quit for States
void terminate_state_loop ();

#endif //_STATE_H_
