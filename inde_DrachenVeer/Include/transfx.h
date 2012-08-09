#ifndef _TRANSFX_H_
#define _TRANSFX_H_

//Header file for Transition Effects.

#include <header.h>

//Transition Effect
typedef void (*transfx_function) (ALLEGRO_BITMAP * srcfrom,
				  ALLEGRO_BITMAP * srcto,
				  ALLEGRO_BITMAP * dest, int curf, int totf);

//Transition Render Stop for States
void trans_stop ();

//Transition Render Start for States
void trans_start (transfx_function trans, int totf);

//Wipe Effect
void wipe (ALLEGRO_BITMAP * srcfrom, ALLEGRO_BITMAP * srcto,
	   ALLEGRO_BITMAP * dest, int curf, int totf);

//Shrink Effect
void shrink (ALLEGRO_BITMAP * srcfrom, ALLEGRO_BITMAP * srcto,
	     ALLEGRO_BITMAP * dest, int curf, int totf);

//Flicker Effect
void flicker (ALLEGRO_BITMAP * srcfrom, ALLEGRO_BITMAP * srcto,
	     ALLEGRO_BITMAP * dest, int curf, int totf);

//Stripe Effect
void stripe (ALLEGRO_BITMAP * srcfrom, ALLEGRO_BITMAP * srcto,
	     ALLEGRO_BITMAP * dest, int curf, int totf);

#endif //_TRANSFX_H_
