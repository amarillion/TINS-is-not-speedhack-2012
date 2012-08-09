#ifndef __drakes_h__
#define __drakes_h__

#include <algorithm>
#include <cstdio>
#include <queue>
#include <string>
#include <utility>
#include <vector>

using namespace std;

#include <allegro5/allegro.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

typedef double real;

inline void do_nothing (void)
{
}

extern const int MAX_X, MAX_Y;

extern ALLEGRO_FONT * global_font_18;
extern ALLEGRO_FONT * global_font_24;
extern ALLEGRO_FONT * global_font_72;

extern ALLEGRO_COLOR border_color;
extern ALLEGRO_COLOR fixed_color;
extern ALLEGRO_COLOR background_color;

extern int max_prime [];
extern int max_score;

extern ALLEGRO_BITMAP * dragon_flame;
extern ALLEGRO_BITMAP * dragon;
extern ALLEGRO_BITMAP * flame;

#endif // __drakes_h__
