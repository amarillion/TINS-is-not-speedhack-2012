//Source file for driver program.

#include <defs.h>
#include <draw.h>
#include <level.h>
#include <log.h>
#include <transfx.h>
#include <state.h>
#include <gameglobal.h>
#include <emu3d.h>
#include <util.h>
#include <math.h>
#include <script.h>
#include <physics.h>
#include <object.h>
#include <fonts.h>
#include <camera.h>
#include <gui.h>
#include <actions.h>

//Definitions.
#define GUI_COMPO_QUIT 0
#define GAMEPLAY_GUI_COMPOSITES 1

//Variables.
static int levelwidth, levelheight;
static int lcamxmin, lcamymin;
static int lcamxmax, lcamymax;
static int camxsp = 0, camysp = 0;
static int camx = 0, camy = 0;
static int *data = NULL;
static int level_render_flags = LEVRENDER_NORMAL | LEVRENDER_HEALTHBARS;
static int emu3d_needed = EMU3D_NULL;
static int game_needs_redraw = 0;
static int level_switching = 0;
static int show_log = 0;
static float prev_time = -1;
static void *gui_composites[GAMEPLAY_GUI_COMPOSITES];
static char statusmsg[MAXSTR];
static int show_healthbars = 1;

//Forward declaration
void cb_quit_no ();
void cb_quit_yes ();
void landed_player (object * hero, void *data);

//Unload Level
void
unload_current_level ()
{
  if (!nlevels)
    return;
#ifdef VERBOSE
  log_printf ("Unloading Level %d: %s", curlevel, levelnames[curlevel]);
#endif //VERBOSE
  unload_level (levels[curlevel]);
  levels[curlevel] = NULL;
}

//Unload Level
void
unload_current_level_and_switch (int newlevel)
{
  int oldlevel = curlevel;
  if (!nlevels)
    return;
  if (newlevel == oldlevel)
    return;
#ifdef VERBOSE
  log_printf ("Switching Level %d: %s", newlevel, levelnames[newlevel]);
#endif //VERBOSE
  curlevel = newlevel;
#ifdef VERBOSE
  log_printf ("Unloading Level %d: %s", oldlevel, levelnames[oldlevel]);
#endif //VERBOSE
  unload_level (levels[oldlevel]);
  levels[oldlevel] = NULL;
}

//Level PreInitialize
void
pre_level_init (int levelnum)
{
  attrmap *am = NULL;
#ifdef VERBOSE
  log_printf ("Trying to Preinitialize Level %d: %s", levelnum,
	      levelnames[levelnum]);
#endif //VERBOSE

  //Create a movable camera
  camx = 0;
  camy = 0;
  if (levels[levelnum]->cam)
    camera_destroy (levels[levelnum]->cam);
  levels[levelnum]->cam = camera_create ((float) camx, (float) camy);

  am = levels[levelnum]->am;
  if (!am)
    return;
  levelwidth = am->width * am->tilewidth;
  levelheight = am->height * am->tileheight;

  lcamxmin = 0;
  lcamymin = 0;

  lcamxmax = levelwidth - 1;
  if (lcamxmax < lcamxmin)
    lcamxmax = lcamxmin;
  lcamymax = levelheight - 1;
  if (lcamymax < 0)
    lcamymax = lcamymin;

  camx = lcamxmin;
  camy = lcamymin;

  data = am->data;

  game_needs_redraw = 1;

#ifdef VERBOSE
  log_printf ("Preinitialized Level %d: %s", levelnum, levelnames[levelnum]);
#endif //VERBOSE
}

//First Level
void
first_level ()
{
  if (!nlevels)
    return;
  curlevel = 0;
#ifdef LOAD_LEVEL_DEFERRED
#ifdef VERBOSE
  log_printf ("Loading Level %d: %s", curlevel, levelnames[curlevel]);
#endif //VERBOSE
  levels[curlevel] = NULL;
  levels[curlevel] = load_level (curlevel);
#endif //LOAD_LEVEL_DEFERRED
  pre_level_init (curlevel);
  log_printf ("First Level");
}

//Next Level
void
next_level ()
{
  int to_load_level;
  if (!nlevels)
    return;
  to_load_level = curlevel + 1;
  if (to_load_level >= nlevels)
    to_load_level = 0;
#ifdef LOAD_LEVEL_DEFERRED
#ifdef VERBOSE
  log_printf ("Loading Level %d: %s", to_load_level,
	      levelnames[to_load_level]);
#endif //VERBOSE
  gui_reset();
  levels[to_load_level] = load_level (to_load_level);
  pre_level_init (to_load_level);
  unload_current_level_and_switch (to_load_level);
#else
  pre_level_init (to_load_level);
  curlevel = to_load_level;
#endif //LOAD_LEVEL_DEFERRED
  log_printf ("Next Level, @Level %d", curlevel);
}

#define CAMACCEL (11)

//Game Initialization
int
game_play_init ()
{
  int status = -1;

//Quit Confirm dialog
  gui_composites[GUI_COMPO_QUIT] =
    gui_messagebox_yesno ("Confirm",
			  "All unsaved progress will be lost.\nDid you mean to quit?",
			  cb_quit_yes, cb_quit_no);

//Load Levels
  if ((status = load_levels ()))
    return status;

//Initialize Emu-3D Renderer
  if ((status = init_emu3d_renderer ()))
    return status;
  emu3d_render_mode (emu3d_needed);

//Initialize Level Animater
  if ((status = init_level_animater ()))
    return status;

//Initialize Level Renderer
  if ((status = init_level_renderer ()))
    return status;

  first_level ();

  return 0;
}

//Game Quit
void
game_play_quit ()
{
//Destroy Camera.
  if (levels && levels[curlevel] && levels[curlevel]->cam)
    {
      camera_destroy (levels[curlevel]->cam);
      levels[curlevel]->cam = NULL;
    }

//Quit Level Renderer
  quit_level_renderer ();

//Quit Level Animater
  quit_level_animater ();

#ifdef LOAD_LEVEL_DEFERRED
  unload_current_level ();
#endif //LOAD_LEVEL_DEFERRED

//Quit Emu-3D Renderer
  quit_emu3d_renderer ();

//Unload Levels
  unload_levels ();
}

//Camera Left
void
left_camera ()
{
  camera *cam = (camera *)levels[curlevel]->cam;
  if(!cam)
    return;
  camx = camera_get_x (cam);
  if (camxsp > 0)
    camxsp = -camxsp;
  if (cam->isAnchored)
    {
      float dx = fabs (camx + camxsp - cam->anchor->x);
      if (dx > CAMERA_TETHER)
	return;
    }
  if (camxsp < 0)
    camx += camxsp;
  if (camx < lcamxmin)
    {
      camx = lcamxmin;
    }
  camxsp--;
  if (camxsp < -CAMACCEL)
    camxsp = -CAMACCEL;
  camera_detach (cam);
  camera_set_x (cam, camx);
  game_needs_redraw = 1;
}

//Camera Right
void
right_camera ()
{
  camera *cam = (camera *)levels[curlevel]->cam;
  if(!cam)
    return;
  camx = camera_get_x (cam);
  if (camxsp < 0)
    camxsp = -camxsp;
  if (cam->isAnchored)
    {
      float dx = fabs (camx + camxsp - cam->anchor->x);
      if (dx > CAMERA_TETHER)
	{
	  return;
	}
    }
  if (camxsp > 0)
    camx += camxsp;
  if (camx > lcamxmax)
    {
      camx = lcamxmax;
    }
  camxsp++;
  if (camxsp > CAMACCEL)
    camxsp = CAMACCEL;
  camera_detach (cam);
  camera_set_x (cam, camx);
  game_needs_redraw = 1;
}

//Camera Up
void
up_camera ()
{
  camera *cam = (camera *)levels[curlevel]->cam;
  if(!cam)
    return;
  camy = camera_get_y (cam);
  if (camysp > 0)
    camysp = -camysp;
  if (cam->isAnchored)
    {
      float dy = fabs (camy + camysp - cam->anchor->y);
      if (dy > CAMERA_TETHER)
	return;
    }
  if (camysp < 0)
    camy += camysp;
  if (camy < lcamymin)
    {
      camy = lcamymin;
    }
  camysp--;
  if (camysp < -CAMACCEL)
    camysp = -CAMACCEL;
  camera_detach (cam);
  camera_set_y (cam, camy);
  game_needs_redraw = 1;
}

//Camera Down
void
down_camera ()
{
  camera *cam = (camera *)levels[curlevel]->cam;
  if(!cam)
    return;
  camy = camera_get_y (cam);
  if (camysp < 0)
    camysp = -camysp;
  if (cam->isAnchored)
    {
      float dy = fabs (camy + camysp - cam->anchor->y);
      if (dy > CAMERA_TETHER)
	return;
    }
  if (camysp > 0)
    camy += camysp;
  if (camy > lcamymax)
    {
      camy = lcamymax;
      camysp = 0;
    }
  camysp++;
  if (camysp > CAMACCEL)
    camysp = CAMACCEL;
  camera_detach (cam);
  camera_set_y (cam, camy);
  game_needs_redraw = 1;
}

//Player landed on a grounding object
void
landed_player (object * hero, void *data)
{
  physics_setjump (levels[curlevel]->physics, hero->physicsdata, 0);
  sprite_set_state (hero->sprinstance, SPRITESET_IDLE);
  game_needs_redraw = 1;
}

//Quit - No
void
cb_quit_no ()
{
  gui_enable (gui_composites[GUI_COMPO_QUIT], 0);
  gui_visible (gui_composites[GUI_COMPO_QUIT], 0);
  al_hide_mouse_cursor (screen);
}

//Quit - Yes
void
cb_quit_yes ()
{
  cb_quit_no ();
  terminate_state_loop ();
}


//Game Input
void
game_play_input (ALLEGRO_EVENT * event)
{
  int code = event->keyboard.keycode;
  object *center = NULL;
  if (level_switching)
    {
      return;
    }
  if (event->type == ALLEGRO_EVENT_KEY_CHAR)
    {
      switch (code)
	{
	case ALLEGRO_KEY_I:
	  {
	    up_camera ();
	    break;
	  }
	case ALLEGRO_KEY_K:
	  {
	    down_camera ();
	    break;
	  }
	case ALLEGRO_KEY_J:
	  {
	    left_camera ();
	    break;
	  }
	case ALLEGRO_KEY_L:
	  {
	    right_camera ();
	    break;
	  }
	case ALLEGRO_KEY_W:
	  {
                center = om_get_object_by_name("hero");
		game_needs_redraw += action_jump(center, 0);
	    break;
	  }
	case ALLEGRO_KEY_S:
	  {
                center = om_get_object_by_name("hero");
		game_needs_redraw += action_crouch(center, 0);
	    break;
	  }
	case ALLEGRO_KEY_A:
	  {
                center = om_get_object_by_name("hero");
		game_needs_redraw += action_direction_left(center, 0);
	    break;
	  }
	case ALLEGRO_KEY_D:
	  {
                center = om_get_object_by_name("hero");
		game_needs_redraw += action_direction_right(center, 0);
	    break;
	  }
	case ALLEGRO_KEY_P:
	  {
                center = om_get_object_by_name("hero");
		game_needs_redraw += action_attack(center, 0);
	    break;
	  }
	case ALLEGRO_KEY_M:
	  {
        center = om_get_object_by_name("hero");
        if(center) {
        	center->sprinstance = om_create_sprite_instance(center->omanager,"TRex");
        }
	    break;
	  }
	case ALLEGRO_KEY_N:
	  {
        center = om_get_object_by_name("hero");
        if(center) {
        	center->sprinstance = om_create_sprite_instance(center->omanager,"Hero");
        }
	    break;
	  }
	case ALLEGRO_KEY_B:
	  {
        center = om_get_object_by_name("hero");
        if(center) {
        	center->sprinstance = om_create_sprite_instance(center->omanager,"Dragon");
        }
	    break;
	  }
	case ALLEGRO_KEY_ESCAPE:
	  {
	    gui_enable (gui_composites[GUI_COMPO_QUIT], 1);
	    gui_visible (gui_composites[GUI_COMPO_QUIT], 1);
	    al_show_mouse_cursor (screen);
	    break;
	  }
	}
    }
  if (event->type == ALLEGRO_EVENT_KEY_UP)
    {
      switch (code)
	{
	case ALLEGRO_KEY_0:
	  {
            center = om_get_object_by_name("hero");
	    center_camera(center);
	    break;
	  }
	case ALLEGRO_KEY_1:
	case ALLEGRO_KEY_2:
	case ALLEGRO_KEY_3:
	case ALLEGRO_KEY_4:
	case ALLEGRO_KEY_5:
	case ALLEGRO_KEY_6:
	case ALLEGRO_KEY_7:
	case ALLEGRO_KEY_8:
	case ALLEGRO_KEY_9:
	  {
	    char name[MAXSTR];
	    sprintf (name, "%s%d", "enemy", (code - ALLEGRO_KEY_0));
            center = om_get_object_by_name(name);
	    center_camera (center);
	    break;
	  }
	case ALLEGRO_KEY_SPACE:
	  {
	    ++level_switching;
	    trans_start (stripe, 20);
	    break;
	  }
	case ALLEGRO_KEY_T:
	  {
	    emu3d_needed++;	// cycle through modes
	    if (emu3d_needed >= EMU3D_TYPES)
	      emu3d_needed = EMU3D_NULL;
	    emu3d_render_mode (emu3d_needed);
	    game_needs_redraw = 1;
	    break;
	  }
	case ALLEGRO_KEY_U:
	  {
	    level_render_flags ^= LEVRENDER_DEBUG_OBJECTS;
	    level_render_flags ^= LEVRENDER_DEBUG_PHYSICS;
	    game_needs_redraw = 1;
	    break;
	  }
	case ALLEGRO_KEY_Y:
	  {
	    show_log = 1 - show_log;
	    game_needs_redraw = 1;
	    break;
	  }
	case ALLEGRO_KEY_H:
	  {
	    show_healthbars = 1 - show_healthbars;
    	level_render_flags ^= LEVRENDER_HEALTHBARS;
	    game_needs_redraw = 1;
	    break;
	  }
	case ALLEGRO_KEY_W:
	  {
            center = om_get_object_by_name("hero");
	    game_needs_redraw += action_jump(center, 1);
            center_camera (center);
	    break;
	  }
	case ALLEGRO_KEY_S:
	  {
            center = om_get_object_by_name("hero");
	    game_needs_redraw += action_crouch(center, 1);
            center_camera (center);
	    break;
	  }
	case ALLEGRO_KEY_A:
	  {
            center = om_get_object_by_name("hero");
	    game_needs_redraw += action_direction_left(center, 1);
            center_camera (center);
	    break;
	  }
	case ALLEGRO_KEY_D:
	  {
            center = om_get_object_by_name("hero");
	    game_needs_redraw += action_direction_right(center, 1);
            center_camera (center);
	    break;
	  }
	case ALLEGRO_KEY_P:
	  {
                center = om_get_object_by_name("hero");
		game_needs_redraw += action_attack(center, 1);
	    break;
	  }
	}
    }
}

//Center camera on an object
void center_camera (void *ptr)
{
  object *optr = (object *)ptr;
  if (NULL != optr && levels && levels[curlevel] && levels[curlevel]->cam)
    {
      camera_set_anchor (levels[curlevel]->cam, optr);
      camera_set_returnToAnchor (levels[curlevel]->cam, 1);
      game_needs_redraw = 1;
    }
}

//Game Logic
void
game_play_logic ()
{
  float now, dt;
  if (level_switching)
    {
      next_level ();
      level_switching = 0;
    }
  else
    {
      if (-1 == prev_time)
      {
        prev_time = al_get_time ();
        return;
      }
      now = (float) al_get_time ();
      dt = now - prev_time;
      game_needs_redraw += om_update(levels[curlevel]->objectmanager, dt);

      //Move camera
      camera_update (levels[curlevel]->cam, dt);
      {
        int oldcamx = camx, oldcamy = camy;
        camx = (int) (camera_get_x (levels[curlevel]->cam) + 0.5f);
        camy = (int) (camera_get_y (levels[curlevel]->cam) + 0.5f);

        //Sanitize camera, especially if following.
        if (camx < lcamxmin)
          camx = lcamxmin;
        if (camx > lcamxmax)
          camx = lcamxmax;
        if (camy < lcamymin)
          camy = lcamymin;
        if (camy > lcamymax)
          camy = lcamymax;
        if (oldcamx != camx || oldcamy != camy)
          game_needs_redraw++;

      }

      //Update time
      prev_time = now;

      //Level animation
      game_needs_redraw += animate_level (levels[curlevel], camx, camy, 0);

#if 0
#define ATTACK_RADIUS 150.0f
      //status update
      {
		  object *hero = om_get_object_by_name ("hero");
		  objectmanager *om = levels[curlevel]->objectmanager;
		  objectptr_iterator oi, closest_obj = NULL;
		  object *obj;
		  float dx, dy, dist, mindist = xres;
		  int valid, health;
		  if (NULL != hero) {
			health = hero->health;
			if(hero->sprinstance->cursst == SPRITESET_ATTACK) {
				//Beginning of attack
				if(hero->sprinstance->curframe == 0) {
					//Find closest enemy
					for(oi=om->begin; oi!=om->end; ++oi) {
						obj = *oi;
						if(!obj || obj->garbage || obj->type!=OBJECT_ENEMY)
							continue;
						dx = (obj->x + obj->w/2 - hero->x - hero->w/2);
						dy = (obj->y + obj->h/2 - hero->y - hero->h/2);
						dist = sqrt(dx*dx + dy*dy);
						valid = 0;
						if((dx > 0) && (obj->facing == OBJECT_FACING_E))
							valid = 1;
						if((dx < 0) && (obj->facing == OBJECT_FACING_W))
							valid = 1;
						if(valid && (mindist > dist)) {
							closest_obj = oi;
							mindist = dist;
						}
					}
					if(closest_obj && (mindist <= ATTACK_RADIUS) ) {
						obj = *closest_obj;
						obj->health = 0.0f;
						obj->state = -1;
						sprite_set_state(obj->sprinstance,SPRITESET_DEAD);
					}
				}
				//End of attack
				if(hero->sprinstance->curframe == hero->sprinstance->sprite->spritesets[hero->sprinstance->cursst]->nframes - 1) {
					sprite_set_state(hero->sprinstance, SPRITESET_IDLE);
				}
			}
		  }
      }
#endif
      sprintf (statusmsg, "WSAD/IJKL/Space Camera: %4d,%4d", camx, camy);

    }
}

static float prev_health = 100.0f;
static int health_anim = 0;
static float health_diff = 0.0f;
static double health_anim_start = 0.0, health_anim_curr = 0.0, health_anim_end = 0.0;

void
transform_coords_render(object *hero, int *stx, int *sty) {
	float hx, hy;
	float xoff, yoff;
	transform_coords(rbuffer, levels[curlevel], camx, camy, hero->layer, stx, sty);
	hx = hero->x;
	hy = hero->y;
	xoff = 0.5f*xres;
	yoff = 0.5f*yres;
	if((hx > xoff) && (hx < levels[curlevel]->am->width - xoff))
		hx -= *stx;
	if((hy > yoff) && (hy < levels[curlevel]->am->height - yoff))
		hy -= *sty - yoff;
	*stx = (int)hx;
	*sty = (int)hy;
}

//Game Render
void
game_play_render ()
{
  if (level_switching)
    {
      if (show_log)
	display_log ();
      else
	skip_render ();
      return;
    }
  if (game_needs_redraw)
    {
      render_level (rbuffer, levels[curlevel], camx, camy, level_render_flags);
      if (show_log)
    	  display_log ();

      {
	ALLEGRO_FONT *font = fm_get_font (FONT_TEXT);
	if (font)
	  {
	    int font_height = al_get_font_line_height (font);
	    al_draw_filled_rectangle (0.0f, yres - font_height, xres, yres, al_map_rgba (0, 0, 0, 200));
	    al_draw_text (font, al_map_rgb (255, 255, 255), 0.0f, (float)(yres - font_height), ALLEGRO_ALIGN_LEFT, statusmsg);
	  }
		font = fm_get_font (FONT_SCOREO);
		if(font) {
		    int font_height = al_get_font_line_height (font);
			char scoremsg[MAXSTR];
			sprintf(scoremsg,"%d",score);
			al_draw_text (font, al_map_rgb (0, 0, 0), (float)(xres - font_height) + 7.0f, (float)(yres - 2.0f*font_height + 6.0f), ALLEGRO_ALIGN_RIGHT, scoremsg);
		}
		font = fm_get_font (FONT_SCORE);
		if(font) {
		    int font_height = al_get_font_line_height (font);
			char scoremsg[MAXSTR];
			sprintf(scoremsg,"%d",score);
			al_draw_text (font, al_map_rgb (64, 64, 64), (float)(xres - font_height), (float)(yres - 2.0f*font_height), ALLEGRO_ALIGN_RIGHT, scoremsg);
		}
		//Health animation
		{
			object* hero = om_get_object_by_name("hero");
			int hx, hy;
			double t = (health_anim_curr-health_anim_start);
			if(hero) {
				if(health_anim) {
					health_anim_curr = al_get_time();
					if(health_anim_curr > health_anim_end)
						health_anim = 0;
					char healthmsg[MAXSTR];
					sprintf(healthmsg,"%d",(int)health_diff);
					transform_coords_render(hero, &hx, &hy);
					al_draw_text (font, al_map_rgb (0,0,0), hx, hy - 50.0f*t - 50.0f*t*t, ALLEGRO_ALIGN_CENTER, healthmsg);
				}
				if(prev_health != hero->health) {
					if(health_anim) {
						health_anim = 0;
					}
					else {
						health_anim = 1;
						health_anim_start = al_get_time();
						health_anim_end = health_anim_start + 1.0;
						health_diff = hero->health - prev_health;
						prev_health = hero->health;
					}
				}
			}
		}
      }
      game_needs_redraw = 0;
    }
  else
    {
      skip_render ();
    }
}
