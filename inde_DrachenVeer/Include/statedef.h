#ifndef _STATEDEF_H_
#define _STATEDEF_H_

//Header file for State Callbacks.

#include <state.h>
#include <object.h>

//Initializes Screen
int init_screen ();

//Game Shutdown
void game_shutdown ();

//Usual Unimplemented
int usual_init ();
void usual_logic ();
void usual_input (ALLEGRO_EVENT * event);
void usual_render ();
void usual_quit ();

//During GamePlay
int game_play_init ();
void game_play_logic ();
void game_play_input (ALLEGRO_EVENT * event);
void game_play_render ();
void game_play_quit ();
void game_play_center_camera (object *);

//During SplashScreen
int splash_init ();
void splash_logic ();
void splash_input (ALLEGRO_EVENT * event);
void splash_render ();
void splash_quit ();

#endif //_STATEDEF_H_
