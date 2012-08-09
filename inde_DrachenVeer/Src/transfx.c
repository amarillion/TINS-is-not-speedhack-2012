//Source file for Transition Effects.

#include <transfx.h>
#include <draw.h>

//Wipe Effect
void
wipe (ALLEGRO_BITMAP * srcfrom, ALLEGRO_BITMAP * srcto, ALLEGRO_BITMAP * dest,
      int curf, int totf)
{
  int w = al_get_bitmap_width (srcfrom), h = al_get_bitmap_height (srcto);
  int stw = (w * curf) / totf;
  al_set_target_bitmap (dest);
  blit (srcto, 0, 0, 0, 0, stw, h, 0);
  blit (srcfrom, stw + 1, 0, stw + 1, 0, w - stw, h, 0);
}

//Shrink Effect
void
shrink (ALLEGRO_BITMAP * srcfrom, ALLEGRO_BITMAP * srcto,
	ALLEGRO_BITMAP * dest, int curf, int totf)
{
  int w = al_get_bitmap_width (srcfrom), h = al_get_bitmap_height (srcto);
  int stw = (w * curf) / totf, sth = (h * curf) / totf;
  int stx = stw >> 1, sty = sth >> 1;
  al_set_target_bitmap (dest);
  blit (srcto, 0, 0, 0, 0, w, h, 0);
  stretch_blit (srcfrom, 0, 0, w, h, stx, sty, w - stw, h - sth, 0);
}

//Flicker Effect
void
flicker (ALLEGRO_BITMAP * srcfrom, ALLEGRO_BITMAP * srcto,
	 ALLEGRO_BITMAP * dest, int curf, int totf)
{
  int w = al_get_bitmap_width (srcfrom), h = al_get_bitmap_height (srcto);
  al_set_target_bitmap (dest);
//TODO: Set Transparency Here.
  if (curf % 2)
    blit (srcfrom, 0, 0, 0, 0, w, h, 0);
  else
    blit (srcto, 0, 0, 0, 0, w, h, 0);
}

//Stripe Effect
void
stripe (ALLEGRO_BITMAP * srcfrom, ALLEGRO_BITMAP * srcto,
	ALLEGRO_BITMAP * dest, int curf, int totf)
{
  int w = al_get_bitmap_width (srcfrom), h = al_get_bitmap_height (srcto);
  int x = 0, stripe = 0, y = (h * curf / totf);
  const int STRIPEWIDTH = 16;
  al_set_target_bitmap (dest);
  for (x = 0; x < w; x += STRIPEWIDTH)
    {
      if (stripe)
	{
	  blit (srcfrom, x, y, x, y, STRIPEWIDTH, h - y, 0);
	  blit (srcto, x, 0, x, 0, STRIPEWIDTH, y, 0);
	}
      else
	{
	  blit (srcfrom, x, 0, x, 0, STRIPEWIDTH, h - y, 0);
	  blit (srcto, x, h - y, x, h - y, STRIPEWIDTH, y, 0);
	}
      stripe = 1 - stripe;
    }
}
