//Source file for sprite rendering.

#include <level.h>
#include <draw.h>
#include <fonts.h>

//Render sprite.
void render_sprite (ALLEGRO_BITMAP * bitmap, spriteinstance * givsi, int xcursor,
	       int ycursor, float angle, int sprite_flags)
{
  int bxcursor, bycursor;
  int sbmwidth, sbmheight;
  int sbwidth, sbheight;
  int filler;
  spriteset *curset;
  spritesetframe *curssframe;
  ALLEGRO_BITMAP *sprbitmap;
  int smsprwidth;
  int smsprheight;

  sprbitmap = givsi->sprite->spritebitmap;
  smsprwidth = givsi->sprite->spritewidth;
  smsprheight = givsi->sprite->spriteheight;

  sbwidth = al_get_bitmap_width (sprbitmap);
  sbheight = al_get_bitmap_height (sprbitmap);
  sbmwidth = sbwidth / smsprwidth;
  sbmheight = sbheight / smsprheight;

  curset = givsi->sprite->spritesets[givsi->cursst];
  if (!curset)
    return;

  curssframe = (curset->frames) + givsi->curframe;

//Filler Tiles+Overlays
  filler = curssframe->value;
//Find out why the below happens.

  if (filler >= 0)
    {
      int allegro_flags = 0;
      bxcursor = filler % sbmwidth;
      bycursor = filler / sbmwidth;
      bycursor *= smsprheight;
      bxcursor *= smsprwidth;

      if (sprite_flags & RENDER_SPRITE_FLIP_HORZ)
	allegro_flags |= ALLEGRO_FLIP_HORIZONTAL;
      if (sprite_flags & RENDER_SPRITE_FLIP_VERT)
	allegro_flags |= ALLEGRO_FLIP_VERTICAL;
      rotated_blit (sprbitmap, bxcursor, bycursor, xcursor, ycursor,
		    smsprwidth, smsprheight, angle, allegro_flags);
    }

}

