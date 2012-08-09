#ifndef _DRAW_H_
#define _DRAW_H_

//Header file for draw functions.

#include <header.h>
#include <shader.h>

//Sets Current Shader for Draw.
void set_shader (shader * s);

//Unsets Current Shader for Draw.
void unset_shader ();

//Blits a Tinted Bitmap to Screen
void tinted_blit (ALLEGRO_BITMAP * src, ALLEGRO_BITMAP * aux,
		  ALLEGRO_COLOR * color, float sx, float sy, float dx,
		  float dy, float sw, float sh, int flags);

//Blits a Bitmap to Screen
void blit (ALLEGRO_BITMAP * src, float sx, float sy, float dx, float dy,
	   float sw, float sh, int flags);

//Rotates a Bitmap and Blits to Screen
void rotated_blit (ALLEGRO_BITMAP * src, float sx, float sy, float dx,
		   float dy, float sw, float sh, float angle, int flags);

//Stretches a Bitmap and Draws to Screen
void stretch_blit (ALLEGRO_BITMAP * src, float sx, float sy, float sw,
		   float sh, float dx, float dy, float dw, float dh, int flags);

#endif //_DRAW_H_
