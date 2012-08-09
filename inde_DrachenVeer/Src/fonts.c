#include <defs.h>
#include <gameglobal.h>
#include <fonts.h>
#include <log.h>
#include <util.h>

//Text
#define TEXT_FONT "Default"
#define TEXT_FONT_SIZE 11

//UI
#define UI_FONT "Default"
#define UI_FONT_SIZE 12

//Score
#define SCORE_FONT "Default"
#define SCORE_FONT_SIZE 24

//Score outline
#define SCOREO_FONT "Default"
#define SCOREO_FONT_SIZE 28

//Local Flags.
static int _fonts_enabled = 0;
static int _ttf_fonts_enabled = 0;

//Bitmap Font Parameters.
static int _font_range[2] = { 0x20, 0x7F };

//Local Fonts.
ALLEGRO_FONT *_fonts[FONT_TYPES] = { NULL, NULL };

static void
fm_load_font (const char *fontname, int ptsize, int fonttype)
{
  char filepath[MAXSTR];
  if (!_fonts_enabled)
    return;
  if ((fonttype < 0) || (fonttype >= FONT_TYPES))
    return;
  if (NULL != _fonts[fonttype])
    {
      al_destroy_font (_fonts[fonttype]);
      _fonts[fonttype] = NULL;
    }
  if (_ttf_fonts_enabled)
    {
      *filepath = 0;
      sprintf (filepath, "%s%c%s%c%s%c%s%s", DATADIR, PATHSEP, "Fonts",
	       PATHSEP, fontname, PATHSEP, fontname, ".ttf");
      if (file_exists (filepath))
	{
	  _fonts[fonttype] = al_load_font (filepath, ptsize, 0);
	}
    }
  if (NULL == _fonts[fonttype])
    {
      *filepath = 0;
      sprintf (filepath, "%s%c%s%c%s%c%s%s", DATADIR, PATHSEP, "Fonts",
	       PATHSEP, fontname, PATHSEP, fontname, ".png");
      if(!file_exists(filepath))
      sprintf (filepath, "%s%c%s%c%s%c%s%s", DATADIR, PATHSEP, "Fonts",
	       PATHSEP, fontname, PATHSEP, fontname, ".bmp");
      if(!file_exists(filepath)) {
      *filepath = 0;
      sprintf (filepath, "%s%c%s%c%s%c%s%s", DATADIR, PATHSEP, "Fonts",
	       PATHSEP, fontname, PATHSEP, fontname, ".pcx");
      }
      if(!file_exists(filepath)) {
      *filepath = 0;
      sprintf (filepath, "%s%c%s%c%s%c%s%s", DATADIR, PATHSEP, "Fonts",
	       PATHSEP, fontname, PATHSEP, fontname, ".png");
      }
      if (file_exists (filepath))
	{
	  if (NULL == _fonts[fonttype])
	    {
	      ALLEGRO_BITMAP *font_bitmap;
              int nbmpflags = al_get_new_bitmap_flags();
              al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
              font_bitmap = load_bitmap_ex (filepath, NULL);
              al_set_new_bitmap_flags(nbmpflags);
	      if (font_bitmap)
		{
		  int num_ranges;
		  #ifdef VERBOSE
                 log_printf ("Loading Bitmap Font: %s from File: %s", fontname, filepath);
          #endif //VERBOSE
		  num_ranges = array_length (_font_range) / 2;
		  _fonts[fonttype] = al_grab_font_from_bitmap (font_bitmap, num_ranges, _font_range);
		  al_destroy_bitmap (font_bitmap);
		  font_bitmap = NULL;
		}
	    }
	}
    }
#ifdef VERBOSE
  if (_fonts[fonttype])
    {
      log_printf ("Loaded Font: %s of Size: %d", fontname, ptsize);
    }
  else
    {
      log_printf ("Fail to Load Font: %s", fontname);
    }
#endif //VERBOSE
}

int
fm_init ()
{
  int status = -1;
  al_init_font_addon ();
  if (true == (status = al_init_ttf_addon ()))
    {
      _ttf_fonts_enabled = 1;
    }
  _fonts_enabled = 1;
  fm_load_font (UI_FONT, UI_FONT_SIZE, FONT_UI);
  fm_load_font (TEXT_FONT, TEXT_FONT_SIZE, FONT_TEXT);
  fm_load_font (SCORE_FONT, SCORE_FONT_SIZE, FONT_SCORE);
  fm_load_font (SCOREO_FONT, SCOREO_FONT_SIZE, FONT_SCOREO);
  return 0;
}

void
fm_cleanup ()
{
  int f;
  for (f = 0; f < FONT_TYPES; ++f)
    {
      if (NULL != _fonts[f])
	{
	  al_destroy_font (_fonts[f]);
	  _fonts[f] = NULL;
	}
    }
  _ttf_fonts_enabled = 0;
  _fonts_enabled = 0;
}

ALLEGRO_FONT *
fm_get_font (int fonttype)
{
  if ((0 == _fonts_enabled) || (fonttype < 0) || (fonttype >= FONT_TYPES))
    return NULL;
  return _fonts[fonttype];
}
