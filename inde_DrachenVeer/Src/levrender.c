//Source file for level rendering.

#include <global.h>
#include <gameglobal.h>
#include <level.h>
#include <log.h>
#include <emu3d.h>
#include <draw.h>
#include <object.h>
#include <fonts.h>

#define OBJECTMAP_SPEED 50

//Uninitialize Level Renderer
void
quit_level_renderer ()
{
  log_printf ("Quitting Level Renderer");
}

//Initialize Level Renderer
int
init_level_renderer ()
{
  log_printf ("Initializing Level Renderer");
  return 0;
}

//Render TileSet
void
render_tileset (ALLEGRO_BITMAP * bitmap, ALLEGRO_BITMAP * tilebitmap,
		tileset * givts, int xcursor, int ycursor, int tmtilewidth,
		int tmtileheight, int flags)
{
  int bxcursor, bycursor;
  int filler, top, bottom, left, right, tl, tr, bl, br;
  int tbmwidth, tbmheight;
  int tbwidth, tbheight;
  tilesetframe *curtsframe;

  tbwidth = al_get_bitmap_width (tilebitmap);
  tbheight = al_get_bitmap_height (tilebitmap);
  tbmwidth = tbwidth / tmtilewidth;
  tbmheight = tbheight / tmtileheight;

  curtsframe = givts->frames + (givts->curframe);

//Filler Tiles+Overlays
  filler = curtsframe->filler;
  top = curtsframe->top;
  bottom = curtsframe->bottom;
  left = curtsframe->left;
  right = curtsframe->right;
  tl = curtsframe->tl;
  tr = curtsframe->tr;
  bl = curtsframe->bl;
  br = curtsframe->br;


  if (filler >= 0)
    {
      bycursor = filler / tbmwidth;
      bxcursor = filler % tbmwidth;
      bycursor *= tmtileheight;
      bxcursor *= tmtilewidth;
      blit (tilebitmap, bxcursor, bycursor, xcursor, ycursor, tmtilewidth,
	    tmtileheight, 0);
    }

  if (left >= 0)
    {
      bycursor = left / tbmwidth;
      bxcursor = left % tbmwidth;
      bycursor *= tmtileheight;
      bxcursor *= tmtilewidth;
      blit (tilebitmap, bxcursor, bycursor, xcursor, ycursor, tmtilewidth,
	    tmtileheight, 0);
    }

  if (right >= 0)
    {
      bycursor = right / tbmwidth;
      bxcursor = right % tbmwidth;
      bycursor *= tmtileheight;
      bxcursor *= tmtilewidth;
      blit (tilebitmap, bxcursor, bycursor, xcursor, ycursor, tmtilewidth,
	    tmtileheight, 0);
    }

  if (bottom >= 0)
    {
      bycursor = bottom / tbmwidth;
      bxcursor = bottom % tbmwidth;
      bycursor *= tmtileheight;
      bxcursor *= tmtilewidth;
      blit (tilebitmap, bxcursor, bycursor, xcursor, ycursor, tmtilewidth,
	    tmtileheight, 0);
    }

  if (top >= 0)
    {
      bycursor = top / tbmwidth;
      bxcursor = top % tbmwidth;
      bycursor *= tmtileheight;
      bxcursor *= tmtilewidth;
      blit (tilebitmap, bxcursor, bycursor, xcursor, ycursor, tmtilewidth,
	    tmtileheight, 0);
    }

  if (bl >= 0)
    {
      bycursor = bl / tbmwidth;
      bxcursor = bl % tbmwidth;
      bycursor *= tmtileheight;
      bxcursor *= tmtilewidth;
      blit (tilebitmap, bxcursor, bycursor, xcursor, ycursor, tmtilewidth,
	    tmtileheight, 0);
    }

  if (br >= 0)
    {
      bycursor = br / tbmwidth;
      bxcursor = br % tbmwidth;
      bycursor *= tmtileheight;
      bxcursor *= tmtilewidth;
      blit (tilebitmap, bxcursor, bycursor, xcursor, ycursor, tmtilewidth,
	    tmtileheight, 0);
    }

  if (tl >= 0)
    {
      bycursor = tl / tbmwidth;
      bxcursor = tl % tbmwidth;
      bycursor *= tmtileheight;
      bxcursor *= tmtilewidth;
      blit (tilebitmap, bxcursor, bycursor, xcursor, ycursor, tmtilewidth,
	    tmtileheight, 0);
    }

  if (tr >= 0)
    {
      bycursor = tr / tbmwidth;
      bxcursor = tr % tbmwidth;
      bycursor *= tmtileheight;
      bxcursor *= tmtilewidth;
      blit (tilebitmap, bxcursor, bycursor, xcursor, ycursor, tmtilewidth,
	    tmtileheight, 0);
    }

}

//Render TileMap
void
render_tilemap (ALLEGRO_BITMAP * bitmap, tilemap * givtm, attrmap * givam,
		int camx, int camy, int flags)
{
  int i, j;

  int xcursor, ycursor;
  int xoffset, yoffset;

  int tilecamx, tilecamy;
  int tilestx, tilesty;
  int tileenx, tileeny;
  int ntilesx, ntilesy;

  int bwidth, bheight;
  int tmwidth, tmheight;
  int tmtilewidth, tmtileheight;
  long tmlevelwidth, tmlevelheight;

  int *hline, *hlinestx;
  int hw, nts, tts;

  ALLEGRO_BITMAP *tilebitmap;
  tileset *ts;

//For Reuse
  if (!givtm)
    return;
  tilebitmap = givtm->tilebitmap;
  tmwidth = givtm->width;
  tmheight = givtm->height;
  tmtilewidth = givtm->tilewidth;
  tmtileheight = givtm->tileheight;
  tmlevelwidth = tmwidth * tmtilewidth;
  tmlevelheight = tmheight * tmtileheight;
  bwidth = al_get_bitmap_width (bitmap);
  bheight = al_get_bitmap_height (bitmap);

//Bring Camera To Map projection
  tilecamx = camx;
  tilecamy = camy;

//Center Drawing Area
  tileenx = tilecamx + (bwidth >> 1);
  tileeny = tilecamy + (bheight >> 1);
  if (tileenx >= tmlevelwidth)
    tileenx = tmlevelwidth - 1;
  if (tileeny >= tmlevelheight)
    tileeny = tmlevelheight - 1;
  tilestx = tileenx - bwidth + 1;
  tilesty = tileeny - bheight + 1;
  if (tilestx < 0)
    tilestx = 0;
  if (tilesty < 0)
    tilesty = 0;

//Calculate for Drawing
  xoffset = tilestx % tmtilewidth;
  yoffset = tilesty % tmtileheight;
  tilestx /= tmtilewidth;
  tilesty /= tmtileheight;
  ntilesx = (bwidth + tmtilewidth - 1) / tmtilewidth;
  ntilesy = (bheight + tmtileheight - 1) / tmtileheight;
  if (tilestx < 0)
    tilestx = 0;
  if (tilesty < 0)
    tilesty = 0;

//Final Clamp
  if (ntilesx > tmwidth)
    ntilesx = tmwidth;
  if (ntilesy > tmheight)
    ntilesy = tmheight;

//Pointer for data access
  hline = givtm->data + (tilesty * tmwidth);
  ts = givtm->tilesets;
  tts = givtm->ntilesets;
  hw = givtm->width;

//Draw Loop with Unroll.
#include <unroll.h>
  ycursor = -yoffset;
  U_unroll(i, ntilesy, { \
      /* Fully Visible */ \
      xcursor = -xoffset; \
      hlinestx = hline + tilestx; \
      U_unroll(j, ntilesx, { \
          /* Fully Visible */ \
	  nts = *hlinestx; \
	  if (nts >= 0 && nts < tts) \
	    { \
	      render_tileset (bitmap, tilebitmap, &ts[nts], xcursor, ycursor, \
			      tmtilewidth, tmtileheight, flags); \
	    } \
	  xcursor += tmtilewidth; \
	  ++hlinestx; \
	} \
      ); \
      if (xoffset && (tilestx + ntilesx) < tmwidth) \
        /* Partially Visible */ \
	{  \
	  nts = *hlinestx; \
	  if (nts >= 0 && nts < tts) \
	    {  \
	      render_tileset (bitmap, tilebitmap, &ts[nts], xcursor, ycursor, \
			      tmtilewidth, tmtileheight, flags); \
	    } \
	} 
      ycursor += tmtileheight; \
      hline += hw; \
    } \
);

  if (yoffset && (tilesty + ntilesy) < tmheight)
    {
      xcursor = -xoffset;
      hlinestx = hline + tilestx;
      U_unroll(j, ntilesx, { \
      /* Partly Visible */ \
	  nts = *hlinestx; \
	  if (nts >= 0 && nts < tts) \
	    { \
	      render_tileset (bitmap, tilebitmap, &(ts[nts]), xcursor, \
			      ycursor, tmtilewidth, tmtileheight, flags); \
	    } \
	  xcursor += tmtilewidth; \
          ++hlinestx; \
	} \
      );
      if (xoffset && (tilestx + ntilesx) < tmwidth)
	{
//Partly Partly Visible
	  nts = *hlinestx;
	  if (nts >= 0 && nts < tts)
	    {
	      render_tileset (bitmap, tilebitmap, &(ts[nts]), xcursor,
			      ycursor, tmtilewidth, tmtileheight, flags);
	    }
	}
    }
#include <unrollundo.h>
}

//Render ParallaxMap
void
render_parallaxmap (ALLEGRO_BITMAP * bitmap, parallaxmap * givpm,
		    attrmap * givam, int camx, int camy, int xspeed,
		    int yspeed, int flags)
{
  int cambx, camby;
  int endbx, endby;
  int startx, starty;
  int pxbwidth, pxbheight;
  int bwidth, bheight;
  int swidth, sheight;
  ALLEGRO_BITMAP *pxbitmap = givpm->pxbitmap;

//For Reuse
  bwidth = al_get_bitmap_width (bitmap);
  bheight = al_get_bitmap_height (bitmap);
  pxbwidth = al_get_bitmap_width (pxbitmap);
  pxbheight = al_get_bitmap_height (pxbitmap);

//Normalize Camera to Parallax Bitmap
  cambx = camx;
  camby = camy;
  cambx += xspeed;
  camby += yspeed;

//Center Drawing Area
  endbx = cambx + (bwidth >> 1);
  endby = camby + (bheight >> 1);
  if (endbx > pxbwidth)
    endbx = pxbwidth;
  if (endby > pxbheight)
    endby = pxbheight;
  startx = endbx - bwidth;
  starty = endby - bheight;
  if (startx < 0)
    startx = 0;
  if (starty < 0)
    starty = 0;
  swidth = bwidth;
  if (swidth > (pxbwidth - startx))
    swidth = (pxbwidth - startx);
  sheight = bheight;
  if (sheight > (pxbheight - starty))
    sheight = (pxbheight - starty);

  if (givpm->alpha >= 255)
    {
      if (swidth != bwidth && sheight != bheight)
	stretch_blit (pxbitmap, startx, starty, swidth, sheight, 0, 0, bwidth,
		      bheight, 0);
      else
	blit (pxbitmap, startx, starty, 0, 0, bwidth, bheight, 0);
    }
}

//Transform To Screen Coords
void transform_coords(ALLEGRO_BITMAP *bitmap, level *givlev, int camx, int camy, int xspeed, int *stxptr, int *styptr) {
//Center Drawing Area
  int bwidth = al_get_bitmap_width (bitmap);
  int bheight = al_get_bitmap_height (bitmap);
  int maxx = (givlev->am->width * givlev->am->tilewidth);
  int maxy = (givlev->am->height * givlev->am->tileheight);
  int enx, eny;
  int stx, sty;

  enx = camx + (xspeed * OBJECTMAP_SPEED) + (bwidth >> 1);
  eny = camy + (bheight >> 1);

  if(enx >= maxx)
	enx = maxx;
  if(eny >= maxy)
	eny = maxy;

  stx = enx - bwidth + 1;
  sty = eny - bheight + 1;

  if(stx < 0) stx = 0;
  if(sty < 0) sty = 0;

  *stxptr = stx;
  *styptr = sty;
}

//Render ObjectMap
void
render_objectmap (ALLEGRO_BITMAP * bitmap, level * givlev, int camx, int camy, int xspeed,
		  int flags)
{
  int nobj, i;
  object *obj;
  objectptr_iterator objit;

  int stx, sty;
  transform_coords(bitmap, givlev, camx, camy, xspeed, &stx, &sty);
  objit = ((objectmanager *)givlev->objectmanager)->begin;
  nobj = om_get_number_of_objects (givlev->objectmanager);

#include <unroll.h>
  U_unroll(i, nobj, {
	obj = *objit; render_object(bitmap, obj, stx, sty, flags); ++objit;
  });
#include <unrollundo.h>

  //Overlay Physics Decals.
   if(givlev->physics && (flags & LEVRENDER_DEBUG_PHYSICS))
   render_physics(bitmap, givlev->physics, stx, sty);

}

//Render ColorMap
void
render_colormap (ALLEGRO_BITMAP * bitmap, colormap * givcm, int flags)
{
  if (givcm->a >= 255)
    {
      al_clear_to_color (al_map_rgb (givcm->r, givcm->g, givcm->b));
    }
}

//Render Level
void
render_level (ALLEGRO_BITMAP * bitmap, level * givlevel, int camx, int camy,
	      int flags)
{
  int i;
  int parallaxdepth = 0;
  int nvisual_layers = 0;
  int has_colormap = 0;
  attrmap *am;
 
  if(!givlevel)
	return;

  al_set_target_bitmap (bitmap);
  al_clear_to_color (al_map_rgb (0, 0, 0));

  am = givlevel->am;

  for (i = 0; i < givlevel->nlayers; ++i)
    {
      if (givlevel->layertypes[i] == LEVEL_PARALLAX
	  || givlevel->layertypes[i] == LEVEL_TILEMAP
	  || givlevel->layertypes[i] == LEVEL_SCRIPT)
	++nvisual_layers;
      if (givlevel->layertypes[i] == LEVEL_COLORMAP)
	++has_colormap;
    }

  al_set_target_bitmap (bitmap);
  if (!has_colormap)
    {
      al_clear_to_color (al_map_rgb (0, 0, 0));
    }

  //Start 3D Emulation
  emu3d_start (nvisual_layers);

  for (i = 0; i < givlevel->nlayers; ++i)
    {
      switch (givlevel->layertypes[i])
	{
	case LEVEL_PARALLAX:	//Render ParallaxMap
	  {
	    parallaxmap *pm = (parallaxmap *) givlevel->layers[i];
	    if (pm)
	      {
		if (pm->speed)
		  render_parallaxmap (bitmap, (parallaxmap *) pm, am, camx,
				      camy, pm->speed, 0, flags);
		else
		  render_parallaxmap (bitmap, (parallaxmap *) pm, am, camx,
				      camy, parallaxdepth++, 0, flags);
	      }
	  }
	  emu3d_layer_done ();
	  break;
	case LEVEL_TILEMAP:	//Render TileMap
	  render_tilemap (bitmap, (tilemap *) givlevel->layers[i], am, camx,
			  camy, flags);
	  emu3d_layer_done ();
	  break;
	case LEVEL_SCRIPT:	//Render Objects
	  render_objectmap (bitmap, givlevel, camx, camy, parallaxdepth++, flags);
	  emu3d_layer_done ();
	  break;
	case LEVEL_COLORMAP:	//Render ColorMap
	  render_colormap (bitmap, (colormap *) givlevel->layers[i], flags);
	  break;
	case LEVEL_ATTRMAP:
	  break;
	case LEVEL_SPRITEMAP:
	  break;
	}
    }

//End 3D Emulation
  emu3d_end ();
}
