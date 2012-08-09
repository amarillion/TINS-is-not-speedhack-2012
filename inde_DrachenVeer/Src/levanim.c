//Source file for animation.

#include <global.h>
#include <level.h>
#include <log.h>

#define LEV_ANIM_LIMIT MSEC_TO_LPS(80)
static volatile int anim_timer;

//Uninitialize Level Animater
void
quit_level_animater ()
{
  log_printf ("Quitting Level Animater");
}

//Initialize Level Animater
int
init_level_animater ()
{
  anim_timer = 0;
  log_printf ("Initializing Level Animater");
  return 0;
}

//Animate TileSet
int
animate_tileset (tileset * givts, int flags)
{
  if (givts->nframes <= 1)
    {
      return 0;
    }
  givts->curframe++;
  if (givts->curframe >= givts->nframes)
    {
      givts->curframe = 0;
    }
  return 1;
}

//Animate TileMap
int
animate_tilemap (tilemap * givtm, attrmap * givam, int camx, int camy,
		 int flags)
{
  int nts = 0;
  int status = 0;
  if (!givtm)
    return status;
  while (nts < givtm->ntilesets)
    {
      status += animate_tileset (givtm->tilesets + nts, flags);
      ++nts;
    }
  return status;
}

//Animate SpriteMap
int
animate_spritemap (spritemap * givsm, attrmap * givam, int camx, int camy,
		   int flags)
{
  int nsi = 0;
  int status = 0;
  if (!givsm)
    return status;
  while (nsi < givsm->nspriteinstances)
    {
      status += animate_sprite (&givsm->spriteinstances[nsi], flags);
      ++nsi;
    }
  return status;
}

//Animate ParallaxMap
int
animate_parallaxmap (parallaxmap * givpm, attrmap * givam, int camx, int camy,
		     int depth, int flags)
{
  return 0;
}

//Animate ColorMap
int
animate_colormap (colormap * givcm, int flags)
{
  return 0;
}

//Animate Level
int
animate_level (level * givlevel, int camx, int camy, int flags)
{
  int i;
  int parallaxdepth = 0;
  int status = 0;

  parallaxmap *pm;
  attrmap *am;

  if ((anim_timer++) % LEV_ANIM_LIMIT)
    {
      return 0;
    }

  am = givlevel->am;
  for (i = 0; i < givlevel->nlayers; ++i)
    {
      switch (givlevel->layertypes[i])
	{
	case LEVEL_COLORMAP:	//Animate ColorMap
	  status +=
	    animate_colormap ((colormap *) givlevel->layers[i], flags);
	  break;
	case LEVEL_PARALLAX:	//Animate ParallaxMap
	  pm = (parallaxmap *) givlevel->layers[i];
	  if (pm)
	    {
	      if (pm->speed)
		status +=
		  animate_parallaxmap ((parallaxmap *) pm, am, camx, camy,
				       pm->speed, flags);
	      else
		status +=
		  animate_parallaxmap ((parallaxmap *) pm, am, camx, camy,
				       parallaxdepth++, flags);
	    }
	  break;
	case LEVEL_ATTRMAP:	//Animate AttributeMap
	  break;
	case LEVEL_TILEMAP:	//Animate TileMap
	  status +=
	    animate_tilemap ((tilemap *) givlevel->layers[i], am, camx, camy,
			     flags);
	  break;
	case LEVEL_SPRITEMAP:	//Animate SpriteMap
	  status +=
	    animate_spritemap ((spritemap *) givlevel->layers[i], am, camx,
			       camy, flags);
	  break;
	}
    }

  return status;
}
