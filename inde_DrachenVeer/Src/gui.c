#include <defs.h>
#include <gameglobal.h>
#include <gui.h>
#include <log.h>
#include <fonts.h>

#include "widgetz/widgetz.h"

/*
 * Implementation notes:
 *
 * 1. There can only be a total of GUI_MAX_WIDGETS callbacks
 * 2. Each widget is usually associated with one callback, e.g. "yes" button => cb_yes()
 * 3. A composite is a collection of widgets used to create a UI item, e.g. a dialog
 * 4. Composites contain a root widget ("container") and references to child widgets
 * 5. Events needs to be passed to render (focus, etc.) the UI elements properly via gui_event()
 * 6. Callbacks are handled manually via unique widget IDs using event.user.data1
 *
 */
/*
 * Usage notes:
 *
 * 1. gui_init(), gui_quit() -- for initialization
 * 2. gui_register_source(), gui_input(), gui_update() -- for GUI event processing
 * 3. gui_render() -- for drawing
 */

#define GUI_DEFAULT_COLOR_TINT 0.3f, 0.2f, 0.1f, 1.0f
#define GUI_DEFAULT_COLOR_FONT 0.9f, 0.9f, 0.9f, 1.0f

WZ_DEF_THEME theme;
WZ_SKIN_THEME theme_skin;
static int use_skin;
WZ_WIDGET *gui = NULL;
ALLEGRO_EVENT_QUEUE *gui_queue = NULL;

typedef struct _gui_composite
{
  int num_widgets;
  WZ_WIDGET *root;
  WZ_WIDGET *widgets[GUI_MAX_WIDGETS_PER_COMPOSITE];
} gui_composite;

static int last_available_composite = -1;
static gui_composite composites[GUI_MAX_COMPOSITES];
static gui_callback gui_callbacks[GUI_MAX_WIDGETS];

void
gui_register_source (ALLEGRO_EVENT_QUEUE * queue)
{
  gui_queue = queue;
}

static void
gui_update_last_available_composite ()
{
  int c;
  if (last_available_composite != -1)	//We've Already Calculated.
    return;
  last_available_composite = -1;
  for (c = 0; c < GUI_MAX_COMPOSITES; ++c)
    {
      if (NULL == composites[c].root)
	{
	  last_available_composite = c;
	  break;
	}
    }
  if (-1 == last_available_composite)
    {
#ifdef VERBOSE
      log_printf
	("All available GUI composites exhausted. Consider increasing GUI_MAX_COMPOSITES.");
#endif //VERBOSE
    }
}

static int
gui_callback_id ()
{
  int c;
  for (c = 0; c < GUI_MAX_WIDGETS * GUI_MAX_WIDGETS_PER_COMPOSITE; ++c)
    {
      if (NULL == gui_callbacks[c])
	break;
    }
  if (c < GUI_MAX_WIDGETS)
    return c;
  return -1;
}

static gui_composite *
gui_composite_init ()
{
  int available_composite;
  gui_composite *compo;
  if (!gui)
    {
      return NULL;
    }
  gui_update_last_available_composite ();
  if (-1 == last_available_composite)
    {
      return NULL;
    }
  available_composite = last_available_composite;
  compo = &composites[available_composite];
  compo->root = wz_create_widget (0, 0, 0, -1);
  wz_attach (compo->root, gui);
  wz_set_theme (compo->root, (0==use_skin)? ((WZ_THEME *) & theme):((WZ_THEME *) & theme_skin));
  compo->num_widgets = 0;
  last_available_composite = -1;
  return compo;
}

static void
gui_composite_destroy (gui_composite * compo)
{
  int w;
  if (!compo)
    return;
  for (w = compo->num_widgets; w >= 0; --w)
    {
      if (compo->widgets[w])
	{
	  compo->widgets[w] = NULL;
	}
    }
  compo->num_widgets = 0;
  if (compo->root)
    {
      wz_detach (compo->root);
      wz_destroy (compo->root);
    }
  compo->root = NULL;
  gui_update_last_available_composite ();
}

static void
gui_destroy_composites ()
{
  int c;
  for (c = 0; c < GUI_MAX_COMPOSITES; ++c)
    {
      gui_composite_destroy (&composites[c]);
    }
  for (c = 0; c < GUI_MAX_WIDGETS; ++c)
    {
      gui_callbacks[c] = NULL;
    }
}

static void
init_composites ()
{
  int c;
//Set composites to NULL to indicate they are all available to be allocated
  for (c = 0; c < GUI_MAX_COMPOSITES; ++c)
    {
      composites[c].root = NULL;
      composites[c].num_widgets = 0;
      {
	int w;
	for (w = 0; w < GUI_MAX_WIDGETS_PER_COMPOSITE; ++w)
	  {
	    composites[c].widgets[w] = NULL;
	  }
      }
    }
  for (c = 0; c < GUI_MAX_WIDGETS; ++c)
    {
      gui_callbacks[c] = NULL;
    }
}

static void
gui_composite_add_widget (gui_composite * compo, void *wgt, int w_id,
			  gui_callback cb)
{
  if (!compo)
    return;
  if (compo->num_widgets >= GUI_MAX_WIDGETS_PER_COMPOSITE)
    return;
  compo->widgets[compo->num_widgets] = wgt;
  if (w_id >= 0)
    gui_callbacks[w_id] = cb;
  ++(compo->num_widgets);
}

void
gui_visible (void *composite, int visible)
{
  gui_composite *compo = (gui_composite *) composite;
  if (compo && compo->root)
    wz_show (compo->root, visible);
}

void
gui_enable (void *composite, int enable)
{
  gui_composite *compo = (gui_composite *) composite;
  if (compo && compo->root)
    wz_enable (compo->root, enable);
}

//Make size variable based on message and title arguments.
void *
gui_messagebox_yesno (char *title, char *message, gui_callback cb_yes,
		      gui_callback cb_no)
{
  int w = 10 + 80 + 10 + 10 + 80 + 10;
  int h = 10 + 50 + 10 + 50 + 10;
  void *wgt;
  int w_id;

// New composite: Dialog
  gui_composite *dialog = gui_composite_init ();
  if (NULL == dialog)
    {
      return NULL;
    }

  gui_composite_add_widget (dialog, wz_create_layout_stop (dialog->root, -1),
			    -1, NULL);
  gui_composite_add_widget (dialog,
			    wz_create_box (dialog->root, xres / 2 - w / 2,
					   yres / 2 - h / 2, w, h, -1), -1,
			    NULL);
  gui_composite_add_widget (dialog,
			    wz_create_textbox (dialog->root, xres / 2 - w / 2,
					       yres / 2 - h / 2 + 5, 200, 50,
					       WZ_ALIGN_CENTRE, WZ_ALIGN_TOP,
					       al_ustr_new (title), 1, -1),
			    -1, NULL);
  gui_composite_add_widget (dialog,
			    wz_create_textbox (dialog->root, xres / 2 - 100,
					       yres / 2 - 40, 200, 50,
					       WZ_ALIGN_CENTRE,
					       WZ_ALIGN_CENTRE,
					       al_ustr_new (message), 1, -1),
			    -1, NULL);
  w_id = gui_callback_id ();
  wgt =
    wz_create_button (dialog->root, xres / 2 - 80 - 10, yres / 2 + 20, 80, 32,
		      al_ustr_new ("Yes"), 1, w_id);
  gui_composite_add_widget (dialog, wgt, w_id, cb_yes);
  w_id = gui_callback_id ();
  wz_create_button (dialog->root, xres / 2 + 10, yres / 2 + 20, 80, 32,
		    al_ustr_new ("No"), 1, w_id);
  gui_composite_add_widget (dialog, wgt, w_id, cb_no);
  wz_register_sources (dialog->root, gui_queue);

//Default: inactive and hidden
  gui_enable (dialog, 0);
  gui_visible (dialog, 0);
  return dialog;
}

void
gui_quit ()
{
  if (!gui)
    return;
  if(1==use_skin) {
//	  al_destroy_bitmap(theme_skin.box_bitmap);
//	  al_destroy_bitmap(theme_skin.button_up_bitmap);
//	  al_destroy_bitmap(theme_skin.button_down_bitmap);
//	  al_destroy_bitmap(theme_skin.scroll_track_bitmap);
//	  al_destroy_bitmap(theme_skin.slider_bitmap);
//	  al_destroy_bitmap(theme_skin.editbox_bitmap);
	  wz_destroy_skin_theme(&theme_skin);
	  use_skin = 0;
  }
  gui_destroy_composites ();
  wz_destroy (gui);
  gui = NULL;
}

void
gui_update_skin(ninepatch slice9)
{
//Unload previously loaded ninepatches
  if(1==use_skin) {
//	  al_destroy_bitmap(theme_skin.box_bitmap);
//	  al_destroy_bitmap(theme_skin.button_up_bitmap);
//	  al_destroy_bitmap(theme_skin.button_down_bitmap);
//	  al_destroy_bitmap(theme_skin.scroll_track_bitmap);
//	  al_destroy_bitmap(theme_skin.slider_bitmap);
//	  al_destroy_bitmap(theme_skin.editbox_bitmap);
	  wz_destroy_skin_theme(&theme_skin);
  }
//Disable skinning if NULL (or 0) was passed
  if(NULL == slice9.box) {
	wz_set_theme (gui, (WZ_THEME *) & theme);
	use_skin = 0;
    return;
  }
//Set-up new ninepatches
  theme_skin.box_bitmap = slice9.box;
  theme_skin.button_up_bitmap = slice9.button_up;
  theme_skin.button_down_bitmap = slice9.button_down;
  theme_skin.scroll_track_bitmap = slice9.scroll_track;
  theme_skin.slider_bitmap = slice9.slider;
  theme_skin.editbox_bitmap = slice9.editbox;
  wz_init_skin_theme(&theme_skin);
  wz_set_theme (gui, (WZ_THEME *) & theme_skin);
  use_skin = 1;
}

void
gui_update_skin_from_sprites (spriteinstance *box,
				spriteinstance *button_up,
				spriteinstance *button_down,
				spriteinstance *scroll,
				spriteinstance *slider,
				spriteinstance *edit)
{
	ninepatch slice9;
        if(!box || !box->sprite)
		return;
	slice9.box = box->sprite->spritebitmap;
	slice9.button_up = button_up->sprite->spritebitmap;
	slice9.button_down = button_down->sprite->spritebitmap;
	slice9.scroll_track = scroll->sprite->spritebitmap;
	slice9.slider = slider->sprite->spritebitmap;
	slice9.editbox = edit->sprite->spritebitmap;
	gui_update_skin(slice9);
}

void
gui_reset ()
{
	//Load ninepatch skin bitmaps, if necessary
	  use_skin = 0;

	//Set-up a default theme
	  memset (&theme, 0, sizeof (theme));
	  memcpy (&theme, &wz_def_theme, sizeof (wz_def_theme));

	//Set-up a skin theme
	  memset (&theme_skin, 0, sizeof (theme_skin));
	  memcpy (&theme_skin, &wz_skin_theme, sizeof (wz_skin_theme));

	//Load Font.
	  theme.font = fm_get_font (FONT_UI);
	  theme_skin.theme.font = fm_get_font (FONT_UI);

	//Colors.
	  theme.color1 = al_map_rgba_f (GUI_DEFAULT_COLOR_TINT);
	  theme.color2 = al_map_rgba_f (GUI_DEFAULT_COLOR_FONT);
	  theme_skin.theme.color1 = al_map_rgba_f (GUI_DEFAULT_COLOR_TINT);
	  theme_skin.theme.color2 = al_map_rgba_f (GUI_DEFAULT_COLOR_FONT);

	  wz_set_theme (gui, (WZ_THEME *) & theme);
}

void
gui_init ()
{
//Create Root Element
  gui = wz_create_widget (0, 0, 0, -1);

  gui_reset ();

//Initialize Composites.
  init_composites ();
}

void
gui_update ()
{
  if (gui)
    wz_update (gui, GUI_DT);
}

void
gui_render ()
{
  if (gui)
    wz_draw (gui);
}

//We add WZ_* event processing here by manually invoking callbacks
void
gui_input (ALLEGRO_EVENT * event)
{
  if (gui)
    {
      switch (event->type)
	{
	case WZ_BUTTON_PRESSED:
	  {
	    int cb_index = event->user.data1;
	    if (cb_index >= 0)
	      {
		gui_callback cb = gui_callbacks[cb_index];
		if (cb)
		  cb ();
	      }
	    break;
	  }
	default:
	  wz_send_event (gui, event);
	  break;
	}
    }
}

void
gui_set_theme_color (int which_theme, int which_color, float r, float g, float b, float a)
{
	switch(which_theme) {
	case GUI_THEME_DEFAULT:
		if(GUI_COLOR_TINT == which_color)
			theme.color1 = al_map_rgba_f(r,g,b,a);
		if(GUI_COLOR_FONT == which_color)
			theme.color2 = al_map_rgba_f(r,g,b,a);
		break;
	case GUI_THEME_SKINNED:
		if(GUI_COLOR_TINT == which_color)
			theme_skin.theme.color1 = al_map_rgba_f(r,g,b,a);
		if(GUI_COLOR_FONT == which_color)
			theme_skin.theme.color2 = al_map_rgba_f(r,g,b,a);
		break;
	default:
		break;
	}
}
