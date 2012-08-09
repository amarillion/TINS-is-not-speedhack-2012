#ifndef _FONTS_H_
#define _FONTS_H_

#include <header.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>

//Note: Cannot Share Fonts as Font Instances because A5 loads fonts at Fixed Sizes
#define FONT_TEXT 0
#define FONT_UI 1
#define FONT_SCORE 2
#define FONT_SCOREO 3
#define FONT_TYPES 4

int fm_init ();
ALLEGRO_FONT *fm_get_font (int fonttype);
void fm_cleanup ();


#endif //_FONTS_H_
