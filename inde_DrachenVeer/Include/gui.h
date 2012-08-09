#ifndef _GUI_H_
#define _GUI_H_

#include <header.h>
#include <sprite.h>

#define GUI_THEME_DEFAULT 0
#define GUI_THEME_SKINNED 1
#define GUI_COLOR_TINT 0
#define GUI_COLOR_FONT 1

#define GUI_MAX_WIDGETS_PER_COMPOSITE 8
#define GUI_MAX_COMPOSITES 8
#define GUI_MAX_WIDGETS ((GUI_MAX_COMPOSITES) * (GUI_MAX_WIDGETS_PER_COMPOSITE))
#define GUI_DT 0.1

//Ninepatch components
struct _ninepatch {
	ALLEGRO_BITMAP	*box,
			*button_up,
			*button_down,
			*scroll_track,
			*slider,
			*editbox;
};
typedef struct _ninepatch ninepatch;

//GUI Callbacks.
typedef void (*gui_callback) ();

//Input Initialization.
void gui_register_source (ALLEGRO_EVENT_QUEUE * queue);

//Initialization.
void gui_init ();

void gui_quit ();

//Reset theme.
void gui_reset ();

//Processing.
void gui_update ();

void gui_input (ALLEGRO_EVENT * event);

void gui_render ();

//Composite Functions.
void *gui_messagebox_yesno (char *title, char *message, gui_callback cb_yes,
			    gui_callback cb_no);

void gui_visible (void *composite, int visible);

void gui_enable (void *composite, int enable);

//Switch skins
void gui_update_skin (ninepatch slice9);

//Expose to scripting
void gui_update_skin_from_sprites(spriteinstance *box,
				spriteinstance *button_up,
				spriteinstance *button_down,
				spriteinstance *scroll,
				spriteinstance *slider,
				spriteinstance *edit);

void gui_set_theme_color (int which_theme, int which_color,
		float r, float g, float b, float a);

#endif //_GUI_H_
