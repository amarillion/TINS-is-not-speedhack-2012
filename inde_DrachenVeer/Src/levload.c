//Source file for Level Loading functions.

#include <level.h>
#include <log.h>
#include <util.h>
#include <gameglobal.h>
#include <object.h>
#include <script.h>
#include <stdlib.h>

//Unload TileSet
void
unload_tileset (tileset * givts)
{
  if (!givts)
    return;

//Remove Frames
  if (givts->frames)
    free (givts->frames);
  givts->frames = NULL;
  givts->nframes = 0;
  givts->curframe = 0;

#ifdef DEBUG
  log_printf ("Freed TileSet");
#endif //DEBUG
}

//Load Tileset
void
load_tileset_file (FILE * ftile, tileset * givts)
{
  int i, allocsizeframes = 0, nframes = 0;

  if (!givts)
    return;

  fscanf (ftile, "%d", &nframes);
  if (!nframes)
    return;
  givts->nframes = nframes;
  givts->curframe = 0;

//Create Frames
  allocsizeframes = sizeof (tilesetframe) * (givts->nframes);
  givts->frames = malloc (allocsizeframes);

  if (!givts->frames)
    return;

  memset (givts->frames, TILE_NULL, allocsizeframes);


//Load Data
  for (i = 0; i < givts->nframes; ++i)
    {
      fscanf (ftile, "%d%d%d%d%d%d%d%d%d",
	      &givts->frames[i].filler,
	      &givts->frames[i].top, &givts->frames[i].bottom,
	      &givts->frames[i].left, &givts->frames[i].right,
	      &givts->frames[i].tl, &givts->frames[i].tr,
	      &givts->frames[i].bl, &givts->frames[i].br);
    }

#ifdef DEBUG
  log_printf ("Loaded TileSet with %d Frames", givts->nframes);
#endif //DEBUG
}

//Unload TileMap
void
unload_tilemap (tilemap * givtm)
{
  int i;

  if (!givtm)
    return;

//Remove Each TileSet
  if (givtm->tilesets)
    {
      for (i = 0; i < givtm->ntilesets; ++i)
	{
	  unload_tileset ((givtm->tilesets) + i);
	}
    }

  free (givtm->tilesets);
  givtm->tilesets = NULL;
  givtm->ntilesets = 0;

//Remove Tile Data
  if (givtm->data)
    {
      free (givtm->data);
    }
  givtm->data = NULL;

  givtm->width = 0;
  givtm->height = 0;
  givtm->tilewidth = 0;
  givtm->tileheight = 0;

//Remove TileBitmap
  if (givtm->tilebitmap)
    {
      al_destroy_bitmap (givtm->tilebitmap);
#ifdef VERBOSE
      log_printf ("Removing ALLEGRO_BITMAP");
#endif //VERBOSE
    }

  givtm->tilebitmap = NULL;
  free (givtm);


#ifdef VERBOSE
  log_printf ("Freed TileMap");
#endif //VERBOSE
}

//Load TileMap from a pre opened file
tilemap *
load_tilemap_file (FILE * ftile, char *levelname)
{
  int width = 0, height = 0;
  int tilewidth = 0, tileheight = 0;
  int ntilesets = 0;
  int allocsizets = 0, allocsizetm = 0, allocsizedata;
  int i, j, l;
  char filename[MAXSTR], tilebitmapfilename[MAXSTR];
  tilemap *newtm = NULL;
  ALLEGRO_BITMAP *tilebitmap = NULL;

  if (!levelname)
    return newtm;

//Create TileMap
  allocsizetm = sizeof (tilemap);
  newtm = malloc (allocsizetm);
  if (!newtm)
    return newtm;

  memset (newtm, 0, allocsizetm);	//Initialize values to 0 ;)

  *filename = 0;
  fscanf (ftile, "%s", filename);
  if (!filename || !*filename)
    {
      unload_tilemap (newtm);
      newtm = NULL;
      return newtm;
    }

  *tilebitmapfilename = 0;
  sprintf (tilebitmapfilename, "%s%c%s%c%s%c%s", DATADIR, PATHSEP, "Levels",
	   PATHSEP, levelname, PATHSEP, filename);

//Load Parameters
  fscanf (ftile, "%d%d%d%d", &width, &height, &tilewidth, &tileheight);

  if (width <= 0 || height <= 0)
    {
      unload_tilemap (newtm);
      newtm = NULL;
      return newtm;
    }

  newtm->width = width;
  newtm->height = height;
  newtm->tilewidth = tilewidth;
  newtm->tileheight = tileheight;

#ifdef VERBOSE
  log_printf ("TileMap ALLEGRO_BITMAP: %s", tilebitmapfilename);
#endif //VERBOSE


//Load Bitmap
  tilebitmap = load_bitmap_ex (tilebitmapfilename, NULL);
  if (!tilebitmap)
    {
      log_printf ("Unable to load tile bitmap [%s]", tilebitmapfilename);
      unload_tilemap (newtm);
      newtm = NULL;
      return newtm;
    }

  newtm->tilebitmap = tilebitmap;

//Load TileSet Data
  fscanf (ftile, "%d", &ntilesets);
  if (ntilesets < 0)
    {
      unload_tilemap (newtm);
      newtm = NULL;
      return newtm;
    }

  newtm->ntilesets = ntilesets;

#ifdef VERBOSE
  log_printf ("TileMap TileSets: %d", newtm->ntilesets);
#endif //VERBOSE

//Create TileSets
  allocsizets = sizeof (tileset) * ntilesets;
  newtm->tilesets = malloc (allocsizets);

  if (!newtm->tilesets)
    {
      unload_tilemap (newtm);
      newtm = NULL;
      return newtm;
    }

  memset (newtm->tilesets, 0, allocsizets);	//Initialize values to 0 ;)

//Load TileSets
  for (i = 0; i < ntilesets; i++)
    {
      load_tileset_file (ftile, (newtm->tilesets) + i);
    }

//Create Tile Data
  allocsizedata = sizeof (int) * (newtm->width) * (newtm->height);
  newtm->data = malloc (allocsizedata);

  if (!newtm->data)
    {
      unload_tilemap (newtm);
      newtm = NULL;
      return newtm;
    }

#ifdef VERBOSE
  log_printf ("TileMap Data: %dx%d Tiles: %dx%d", newtm->width, newtm->height,
	      newtm->tilewidth, newtm->tileheight);
#endif //VERBOSE

  memset (newtm->data, TILE_NULL, allocsizedata);	//Initialize values

//Load Tile Data
  for (l = 0, i = 0; i < newtm->height; ++i)
    {
      for (j = 0; j < newtm->width; ++j)
	{
	  fscanf (ftile, "%d", (newtm->data) + (l));
	  if (*((newtm->data) + l) >= newtm->ntilesets)
	    *((newtm->data) + l) = TILE_NULL;
	  l++;
	}
    }

#ifdef VERBOSE
  log_printf ("Loaded TileMap");
#endif //VERBOSE

  return newtm;
}

//Load TileMap from Level File
tilemap *
load_tilemap_level_file (FILE * flevel, char *levelname)
{
  char filename[MAXSTR], tilefilename[MAXSTR];
  tilemap *newtm = NULL;
  FILE *ftile = NULL;

  *filename = 0;
  fscanf (flevel, "%s", filename);
  if (!filename || !*filename)
    return newtm;

  *tilefilename = 0;
  sprintf (tilefilename, "%s%c%s%c%s%c%s", DATADIR, PATHSEP, "Levels",
	   PATHSEP, levelname, PATHSEP, filename);

  ftile = fopen (tilefilename, "r");
  if (!ftile)
    return newtm;

//Loading TileMap
#ifdef VERBOSE
  log_printf ("Loading TileMap from %s", tilefilename);
#endif //VERBOSE

  newtm = load_tilemap_file (ftile, levelname);

  fclose (ftile);
  ftile = NULL;

  return newtm;
}

//Unload SpriteMap
void
unload_spritemap (spritemap * givsm)
{
  int i;

  if (!givsm)
    return;

//Remove Each Sprite
  if (givsm->sprites)
    {
      for (i = 0; i < givsm->nsprites; ++i)
	{
	  if (givsm->sprites[i])
	    {
	      unload_sprite (givsm->sprites[i]);
	      givsm->sprites[i] = NULL;
	    }
	}
    }

  free (givsm->sprites);
  givsm->sprites = NULL;

//Remove Each SpriteName
  if (givsm->spritenames)
    {
      for (i = 0; i < givsm->nsprites; ++i)
	{
	  if (givsm->spritenames[i])
	    {
	      free (givsm->spritenames[i]);
	      givsm->spritenames[i] = NULL;
	    }
	}
    }

  free (givsm->spritenames);
  givsm->spritenames = NULL;

  givsm->nsprites = 0;

  free (givsm);

//Clear all sprite instances
  givsm->nspriteinstances = 0;

#ifdef VERBOSE
  log_printf ("Freed SpriteMap");
#endif //VERBOSE
}

//Load SpriteMap from a pre opened file
spritemap *
load_spritemap_file (FILE * fsprite, char *levelname)
{
  int nsprites = 0;
  spritemap *newsm = NULL;
  int allocsizesm = 0;
  char spritename[MAXSTR];
  int i;

  if (!levelname)
    return newsm;

//Create SpriteMap
  allocsizesm = sizeof (spritemap);
  newsm = malloc (allocsizesm);
  if (!newsm)
    return newsm;

  memset (newsm, 0, allocsizesm);	//Initialize values to 0 ;)

//Load Number of Sprites
  fscanf (fsprite, "%d", &nsprites);

  newsm->nsprites = nsprites;
  newsm->spritenames = malloc (sizeof (char *) * nsprites);
  if (!newsm->spritenames)
    {
      unload_spritemap (newsm);
      newsm = NULL;
      return newsm;
    }

  newsm->sprites = malloc (sizeof (sprite *) * nsprites);
  if (!newsm->sprites)
    {
      unload_spritemap (newsm);
      newsm = NULL;
      return newsm;
    }

  for (i = 0; i < nsprites; ++i)
    {
      newsm->spritenames[i] = NULL;
      newsm->sprites[i] = NULL;
    }

  for (i = 0; i < nsprites; ++i)
    {
      *spritename = 0;
      newsm->spritenames[i] = malloc (sizeof (spritename));
      *(newsm->spritenames[i]) = 0;
      fscanf (fsprite, "%s%s", newsm->spritenames[i], spritename);
      newsm->sprites[i] = load_sprite (spritename);
      if (!newsm->sprites[i])
	{
	  log_printf ("Unable to load sprite [%s]", spritename);
	  unload_spritemap (newsm);
	  newsm = NULL;
	  return newsm;
	}
#ifdef VERBOSE
      log_printf ("Loaded Sprite: %s of type %s", newsm->spritenames[i],
		  spritename);
#endif //VERBOSE
    }

//No instances created yet...will be done by object loader
  newsm->nspriteinstances = 0;

#ifdef VERBOSE
  log_printf ("Loaded SpriteMap");
#endif //VERBOSE

  return newsm;
}


//Load SpriteMap from Level File
spritemap *
load_spritemap_level_file (FILE * flevel, char *levelname)
{
  char filename[MAXSTR], spritefilename[MAXSTR];
  spritemap *newsm = NULL;
  FILE *fsprite = NULL;

  *filename = 0;
  fscanf (flevel, "%s", filename);
  if (!filename || !*filename)
    return newsm;

  *spritefilename = 0;
  sprintf (spritefilename, "%s%c%s%c%s%c%s", DATADIR, PATHSEP, "Levels",
	   PATHSEP, levelname, PATHSEP, filename);

  fsprite = fopen (spritefilename, "r");
  if (!fsprite)
    return newsm;

//Loading spriteMap
#ifdef VERBOSE
  log_printf ("Loading SpriteMap from %s", spritefilename);
#endif //VERBOSE

  newsm = load_spritemap_file (fsprite, levelname);

  fclose (fsprite);
  fsprite = NULL;

  return newsm;
}

//Unload ColorMap 
void
unload_colormap (colormap * givcm)
{
  if (givcm)
    {
      free (givcm);

#ifdef VERBOSE
      log_printf ("Freed ColorMap");
#endif //VERBOSE
    }
}

//Create ColorMap: Red, Green, Blue, Alpha components
colormap *
create_colormap (int r, int g, int b, int a)
{
  colormap *newcm = NULL;
  newcm = malloc (sizeof (colormap));
  if (newcm)
    {
//Initialize values
      newcm->r = r;
      newcm->g = g;
      newcm->b = b;
      newcm->a = a;
#ifdef VERBOSE
      log_printf ("Created ColorMap: R:%d G:%d B:%d A:%d HTML:#%02x%02x%02x",
		  newcm->r, newcm->g, newcm->b, newcm->a, newcm->r, newcm->g,
		  newcm->b);
#endif //VERBOSE
    }
  return newcm;
}

//Load ColorMap from a pre opened file
colormap *
load_colormap_file (FILE * fcolor)
{
  int r = 0, g = 0, b = 0, a = 0;
  colormap *newcm = NULL;
  fscanf (fcolor, "%d%d%d%d", &r, &g, &b, &a);
  newcm = create_colormap (r, g, b, a);
  return newcm;
}

//Load ColorMap from Level File
colormap *
load_colormap_level_file (FILE * flevel, char *levelname)
{
  colormap *newcm = NULL;
  newcm = load_colormap_file (flevel);
  return newcm;
}

//Unload ParallaxMap
void
unload_parallaxmap (parallaxmap * givpm)
{
  if (givpm)
    {
//Free BG Bitmap
      if (givpm->pxbitmap)
	{
#ifdef VERBOSE
	  log_printf ("Removing ALLEGRO_BITMAP");
#endif //VERBOSE
	  al_destroy_bitmap (givpm->pxbitmap);
	}
      givpm->pxbitmap = NULL;
      givpm->alpha = 0;
      givpm->speed = 0;
      free (givpm);
#ifdef VERBOSE
      log_printf ("Freed ParallaxMap");
#endif //VERBOSE
    }
}

//Create ParallaxMap
parallaxmap *
create_parallaxmap (ALLEGRO_BITMAP * pxbitmap, int alpha, int speed)
{
  parallaxmap *newpm = NULL;
  newpm = malloc (sizeof (parallaxmap));
  if (newpm)
    {
//Initialize values
      newpm->pxbitmap = pxbitmap;
      newpm->alpha = alpha;
      newpm->speed = speed;
#ifdef VERBOSE
      log_printf ("Created ParallaxMap");
#endif //VERBOSE
    }
  return newpm;
}

//Load ParallaxMap from Level File
parallaxmap *
load_parallaxmap_level_file (FILE * flevel, char *levelname)
{
  int alpha = 0;
  int speed = 0;
  char filename[MAXSTR], pxbitmapfilename[MAXSTR];
  parallaxmap *newpm = NULL;
  ALLEGRO_BITMAP *pxbitmap = NULL;

  *filename = 0;
  fscanf (flevel, "%s", filename);
  if (!filename || !*filename)
    return newpm;

  fscanf (flevel, "%d", &alpha);
  fscanf (flevel, "%d", &speed);

  *pxbitmapfilename = 0;
  sprintf (pxbitmapfilename, "%s%c%s%c%s%c%s", DATADIR, PATHSEP, "Levels",
	   PATHSEP, levelname, PATHSEP, filename);

  al_set_new_bitmap_flags (ALLEGRO_PIXEL_FORMAT_ANY_32_WITH_ALPHA);
// new in A5: force bitmaps to be in memory
// otherwise they are loaded as video-card textures
//al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);

//Load Bitmap
  pxbitmap = load_bitmap_ex (pxbitmapfilename, NULL);
  if (!pxbitmap)
    return newpm;

//Loading ParallaxMap
#ifdef VERBOSE
  log_printf ("Loading ParallaxMap from ALLEGRO_BITMAP: %s",
	      pxbitmapfilename);
#endif //VERBOSE

  newpm = create_parallaxmap (pxbitmap, alpha, speed);
  return newpm;
}


//Unload AttributeMap 
void
unload_attrmap (attrmap * givam)
{
  if (givam)
    {
      if (givam->data)
	{
	  free (givam->data);
	  givam->data = NULL;
	}
      givam->width = 0;
      givam->height = 0;
      givam->tilewidth = 0;
      givam->tileheight = 0;
      free (givam);
#ifdef VERBOSE
      log_printf ("Freed AttributeMap");
#endif //VERBOSE
    }
}

//Create AttributeMap 
attrmap *
create_attrmap (int width, int height, int tiles)
{
  attrmap *newam = NULL;
  if (width <= 0 || height <= 0)
    return newam;
  newam = malloc (sizeof (attrmap));
  if (newam)
    {
      newam->width = width;
      newam->height = height;
      newam->tilewidth = 0;
      newam->tileheight = 0;	//Remember to read it sometime ;)
      newam->tileheight = 0;	//Remember to read it sometime ;)
      newam->data = NULL;
      if (tiles)
	{
	  int allocsizedata = sizeof (int) * newam->width * newam->height;
	  newam->data = malloc (allocsizedata);
	  if (newam->data)
	    memset (newam->data, ATTR_NULL, allocsizedata);	//Initialize values
	}
      if (!newam->width || !newam->height || (tiles && !newam->data))
	{
	  unload_attrmap (newam);
	  newam = NULL;
	}
      else
	{
#ifdef VERBOSE
	  log_printf ("Created AttributeMap: %dx%d", newam->width,
		      newam->height);
#endif //VERBOSE
	}
    }
  return newam;
}

//Load AttributeMap from a pre opened file
attrmap *
load_attrmap_file (FILE * fattr)
{
  int i, j, l;
  int width = 0, height = 0;
  int tilewidth = 0, tileheight = 0;
  attrmap *newam = NULL;

  fscanf (fattr, "%d%d", &width, &height);
  fscanf (fattr, "%d%d", &tilewidth, &tileheight);

  if (tilewidth < 0 || tileheight < 0)
    newam = create_attrmap (width, height, 0);
  else
    newam = create_attrmap (width, height, 1);

  if (newam)
    {
      newam->tilewidth = tilewidth;
      newam->tileheight = tileheight;
      if (newam->tilewidth < 0 || newam->tileheight < 0)
	{
	  newam->tilewidth = 1;
	  newam->tileheight = 1;
	}
      else
	{
	  for (l = 0, i = 0; i < newam->height; ++i)
	    {
	      for (j = 0; j < newam->width; ++j)
		{
		  fscanf (fattr, "%d", (newam->data) + (l++));
		}
	    }
	}
#ifdef VERBOSE
      log_printf ("Loaded AttributeMap: %dx%d Tiles: %dx%d", newam->width,
		  newam->height, newam->tilewidth, newam->tileheight);
#endif //VERBOSE
    }
  return newam;
}

//Load AttributeMap from Level File
attrmap *
load_attrmap_level_file (FILE * flevel, char *levelname)
{
  attrmap *newam = NULL;
  char filename[MAXSTR], attrfilename[MAXSTR];
  FILE *fattr = NULL;

  if (!levelname || !*levelname)
    return newam;

  *filename = 0;
  fscanf (flevel, "%s", filename);

  if (!filename || !*filename)
    return newam;

  *attrfilename = 0;
  sprintf (attrfilename, "%s%c%s%c%s%c%s", DATADIR, PATHSEP, "Levels",
	   PATHSEP, levelname, PATHSEP, filename);

  fattr = fopen (attrfilename, "r");
  if (!fattr)
    return newam;

//Loading AttributeMap
#ifdef VERBOSE
  log_printf ("Loading AttributeMap from %s", attrfilename);
#endif //VERBOSE

  newam = load_attrmap_file (fattr);

  fclose (fattr);
  fattr = NULL;

  return newam;
}

//Load script from Level File
void *
load_script_level_file (FILE * flevel, char *levelname, level * newlevel)
{
  char filename[MAXSTR], scriptfilename[MAXSTR];

  if (!levelname || !*levelname)
    return NULL;

  *filename = 0;
  fscanf (flevel, "%s", filename);

  if (!filename || !*filename)
    return NULL;

  *scriptfilename = 0;
  sprintf (scriptfilename, "%s%c%s%c%s%c%s", DATADIR, PATHSEP, "Levels",
	   PATHSEP, levelname, PATHSEP, filename);

  script_execute_file (scriptfilename, newlevel);

#ifdef VERBOSE
  log_printf ("Loaded Script");
#endif //VERBOSE

  return NULL;
}

//Load ObjectMap from Level File
void *
load_objmap_level_file (FILE * flevel, char *levelname, level * newlevel)
{
  void *newom = NULL;
  char filename[MAXSTR], objfilename[MAXSTR];
  FILE *fobj = NULL;

  if (!levelname || !*levelname)
    return newom;

  *filename = 0;
  fscanf (flevel, "%s", filename);

  if (!filename || !*filename)
    return newom;

  *objfilename = 0;
  sprintf (objfilename, "%s%c%s%c%s%c%s", DATADIR, PATHSEP, "Levels",
	   PATHSEP, levelname, PATHSEP, filename);

  fobj = fopen (objfilename, "r");
  if (!fobj)
    return newom;

//Loading AttributeMap
#ifdef VERBOSE
  log_printf ("Loading ObjectMap from %s", objfilename);
#endif //VERBOSE

  newom = load_objmap_file (fobj, levelname, newlevel->objectmanager);

  fclose (fobj);
  fobj = NULL;

  return newom;
}

//Unload Level
void
unload_level (level * givlevel)
{
  int i;
  if (givlevel)
    {
      givlevel->am = NULL;
      if (givlevel->physics)
      {
    	  physics_end (givlevel->physics);
      }
      givlevel->physics = NULL;
      for (i = 0; i < givlevel->nlayers; ++i)
	{
	  if (givlevel->layertypes[i] == LEVEL_COLORMAP)
	    {
	      unload_colormap ((colormap *) givlevel->layers[i]);
	      givlevel->layers[i] = (int *) NULL;
	      givlevel->layertypes[i] = LEVEL_NULL;
	    }
	  else if (givlevel->layertypes[i] == LEVEL_PARALLAX)
	    {
	      unload_parallaxmap ((parallaxmap *) givlevel->layers[i]);
	      givlevel->layers[i] = (int *) NULL;
	      givlevel->layertypes[i] = LEVEL_NULL;
	    }
	  else if (givlevel->layertypes[i] == LEVEL_TILEMAP)
	    {
	      unload_tilemap ((tilemap *) givlevel->layers[i]);
	      givlevel->layers[i] = (int *) NULL;
	      givlevel->layertypes[i] = LEVEL_NULL;
	    }
	  else if (givlevel->layertypes[i] == LEVEL_ATTRMAP)
	    {
	      unload_attrmap ((attrmap *) givlevel->layers[i]);
	      givlevel->layers[i] = (int *) NULL;
	      givlevel->layertypes[i] = LEVEL_NULL;
	      givlevel->am = NULL;
	    }
	  else if (givlevel->layertypes[i] == LEVEL_SPRITEMAP)
	    {
	      unload_spritemap ((spritemap *) givlevel->layers[i]);
	      givlevel->layers[i] = (int *) NULL;
	      givlevel->layertypes[i] = LEVEL_NULL;
	      givlevel->sprm = NULL;
	    }
	  else if (givlevel->layertypes[i] == LEVEL_SCRIPT)
	    {
	      givlevel->layers[i] = (int *) NULL;
	      givlevel->layertypes[i] = LEVEL_NULL;
	    }
	  else if (givlevel->layertypes[i] == LEVEL_OBJMAP)
	    {
	      givlevel->layers[i] = (int *) NULL;
	      givlevel->layertypes[i] = LEVEL_NULL;
	    }
	}
      free (givlevel->layertypes);
      givlevel->layertypes = NULL;
      free (givlevel->layers);
      givlevel->layers = NULL;
      givlevel->nlayers = 0;
      givlevel->score = 0;
      if (givlevel->objectmanager)
	{
	  om_end (givlevel->objectmanager);
	}
      givlevel->objectmanager = NULL;
//Destroying Level
      free (givlevel);
#ifdef VERBOSE
      log_printf ("Destroyed Level");
#endif //VERBOSE
    }
}

//Create Level
level *
create_level (int nlayers)
{
  int i;
  level *newlevel = NULL;
  newlevel = malloc (sizeof (level));
  if (newlevel)
    {
      newlevel->nlayers = 0;
      if (nlayers > 0)
	{
#ifdef VERBOSE
	  log_printf ("Created %d layers", nlayers);
#endif //VERBOSE
	  newlevel->nlayers = nlayers;
#ifdef VERBOSE
	  log_printf ("PreInitializing Layer Types");
#endif //VERBOSE
	  newlevel->layertypes = malloc (sizeof (int) * nlayers);
	  if (newlevel->layertypes)
	    {
	      for (i = 0; i < newlevel->nlayers; ++i)
		newlevel->layertypes[i] = LEVEL_NULL;
	    }
#ifdef VERBOSE
	  log_printf ("PreInitializing Layer Data");
#endif //VERBOSE
	  newlevel->layers = malloc (sizeof (void *) * nlayers);
	  if (newlevel->layers)
	    {
	      for (i = 0; i < newlevel->nlayers; ++i)
		newlevel->layers[i] = (int *) NULL;
	    }
	}
      if (!newlevel->nlayers || !newlevel->layertypes || !newlevel->layers)
	{
	  unload_level (newlevel);
	  newlevel = NULL;
	}
      newlevel->am = NULL;
      newlevel->sprm = NULL;
      newlevel->physics = NULL;
      newlevel->objectmanager = NULL;
    }
  return newlevel;
}

//Load Level from a pre opened file
level *
load_level_file (FILE * flevel, char *levelname)
{
  int i;
  int nlayers;
  char layertype[MAXSTR];
  level *newlevel = NULL;

  nlayers = 0;
  fscanf (flevel, "%d", &nlayers);

  newlevel = create_level (nlayers);
  if (newlevel)
    {
      newlevel->physics = physics_start (NULL);
      script_reset ();
      newlevel->objectmanager = om_start (newlevel);
      for (i = 0; i < newlevel->nlayers; ++i)
	{
	  fscanf (flevel, "%s", layertype);
	  if (!strcasecmp ("color", layertype))
	    {
//Color 
#ifdef VERBOSE
	      log_printf ("Layer %d: %s", i, "Color");
#endif //VERBOSE
	      newlevel->layers[i] =
		(int *) load_colormap_level_file (flevel, levelname);
	      newlevel->layertypes[i] = LEVEL_COLORMAP;
	    }
	  else if (!strcasecmp ("parallax", layertype))
	    {
//Parallax
#ifdef VERBOSE
	      log_printf ("Layer %d: %s", i, "Parallax");
#endif //VERBOSE
	      newlevel->layers[i] =
		(int *) load_parallaxmap_level_file (flevel, levelname);
	      newlevel->layertypes[i] = LEVEL_PARALLAX;
	    }
	  else if (!strcasecmp ("attr", layertype))
	    {
//Attributes
#ifdef VERBOSE
	      log_printf ("Layer %d: %s", i, "Attributes");
#endif //VERBOSE
	      newlevel->layers[i] =
		(int *) load_attrmap_level_file (flevel, levelname);
	      newlevel->layertypes[i] = LEVEL_ATTRMAP;
	      if (!newlevel->am)
		newlevel->am = (attrmap *) newlevel->layers[i];
	      physics_load_attributemap (newlevel->physics, newlevel->am);
	    }
	  else if (!strcasecmp ("tiles", layertype))
	    {
//Sprites
#ifdef VERBOSE
	      log_printf ("Layer %d: %s", i, "Tiles");
#endif //VERBOSE
	      newlevel->layers[i] =
		(int *) load_tilemap_level_file (flevel, levelname);
	      newlevel->layertypes[i] = LEVEL_TILEMAP;
	    }
	  else if (!strcasecmp ("sprites", layertype))
	    {
//Sprites
#ifdef VERBOSE
	      log_printf ("Layer %d: %s", i, "Sprites");
#endif //VERBOSE
	      newlevel->layers[i] =
		(int *) load_spritemap_level_file (flevel, levelname);
	      newlevel->layertypes[i] = LEVEL_SPRITEMAP;
	      if (!newlevel->sprm)
		newlevel->sprm = (spritemap *) newlevel->layers[i];
	    }
	  else if (!strcasecmp ("script", layertype))
	    {
	      int *layer = NULL;
//Script
#ifdef VERBOSE
	      log_printf ("Layer %d: %s", i, "Script");
#endif //VERBOSE
	      layer =
		(int *) load_script_level_file (flevel, levelname, newlevel);
	      newlevel->layers[i] = layer;
	      newlevel->layertypes[i] = LEVEL_SCRIPT;
	    }
	  else if (!strcasecmp ("objects", layertype))
	    {
	      int *layer = NULL;
//Objects
#ifdef VERBOSE
	      log_printf ("Layer %d: %s", i, "Objects");
#endif //VERBOSE
	      layer =
		(int *) load_objmap_level_file (flevel, levelname, newlevel);
	      newlevel->layers[i] = layer;
	      newlevel->layertypes[i] = LEVEL_OBJMAP;
	    }
	  else if (!strcasecmp ("objects", layertype))
	    {
	      int *layer = NULL;
//Objects
#ifdef VERBOSE
	      log_printf ("Layer %d: %s", i, "Objects");
#endif //VERBOSE
	      layer =
		(int *) load_objmap_level_file (flevel, levelname, newlevel);
	      newlevel->layers[i] = layer;
	      newlevel->layertypes[i] = LEVEL_OBJMAP;
	    }
	}
    }
  return newlevel;
}

//Load Level from a file, name is identified by 'levelname'
level *
load_level (int levelnum)
{
  char levelfilename[MAXSTR];
  FILE *flevel;
  level *newlevel = NULL;

  if (levelnum < 0 || levelnum >= nlevels)
    return newlevel;
  if (!*levelnames[levelnum])
    return newlevel;

  *levelfilename = 0;
//From: Data/Levels/levelname/level.txt
  sprintf (levelfilename, "%s%c%s%c%s%c%s", DATADIR, PATHSEP, "Levels",
	   PATHSEP, levelnames[levelnum], PATHSEP, "level.txt");

  flevel = fopen (levelfilename, "r");
  if (!flevel)
    return newlevel;

//Loading Level
#ifdef VERBOSE
  log_printf ("Loading Level from: %s", levelfilename);
#endif //VERBOSE

  newlevel = load_level_file (flevel, levelnames[levelnum]);
  newlevel->score = 0;
  newlevel->cam = NULL;

  fclose (flevel);
  return newlevel;
}


//Unload all Levels
void
unload_levels ()
{
  int i;
  if (levels)
    {
//Unloading Levels
#ifdef VERBOSE
      log_printf ("Destroying Levels");
#endif //VERBOSE
      for (i = 0; i < nlevels; ++i)
	{
//Unload Each level
#ifndef LOAD_LEVEL_DEFERRED
#ifdef VERBOSE
	  log_printf ("Unloading Level %d: %s", i, levelnames[i]);
#endif //VERBOSE
//Unload Each level
	  unload_level (levels[i]);
	  levels[i] = NULL;
#endif //LOAD_LEVEL_DEFERRED
	}
      free (levels);
      levels = NULL;
    }
  if (levelnames)
    {
      for (i = 0; i < nlevels; ++i)
	{
	  if (levelnames[i])
	    free (levelnames[i]);
	  levelnames[i] = NULL;
	}
      free (levelnames);
      levelnames = NULL;
    }
  nlevels = 0;
  curlevel = -1;
}

//Load all Levels
int
load_levels ()
{
  int i;
  int status = -1;
  char filename[MAXSTR], levelname[MAXSTR];
  FILE *flevels;
  unload_levels ();

//From: Data/levels.txt
  sprintf (filename, "%s%c%s", DATADIR, PATHSEP, "levels.txt");

  flevels = fopen (filename, "r");
  if (!flevels)
    return status;

//Loading Level
#ifdef VERBOSE
  log_printf ("Loading Levels from list: %s", filename);
#endif //VERBOSE

  nlevels = 0;
  fscanf (flevels, "%d", &nlevels);
  if (!nlevels)
    return -1;
#ifdef VERBOSE
  log_printf ("%d Levels", nlevels);
#endif //VERBOSE

  levels = malloc (sizeof (level *) * nlevels);
  levelnames = malloc (sizeof (char *) * nlevels);
  if (levelnames && levels)
    {
      for (i = 0; i < nlevels; ++i)
	{
	  *levelname = 0;
	  fscanf (flevels, "%s", levelname);
	  strip_crlfs (levelname);
	  if (!(*levelname))
	    {
	      sprintf (levelname, "Level%d", (i + 1));
	    }
	  levelnames[i] = malloc (sizeof (char) * MAXSTR);
	  strncpy (levelnames[i], levelname, MAXSTR);
#ifndef LOAD_LEVEL_DEFERRED
#ifdef VERBOSE
	  log_printf ("Loading Level %d: %s", i, levelname);
#endif //VERBOSE
	  levels[i] = load_level (i);
	  curlevel = i;
#endif //LOAD_LEVEL_DEFERRED
	}
    }
  else
    return status;
  fclose (flevels);
  return 0;
}
