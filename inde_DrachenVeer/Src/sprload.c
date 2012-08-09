//Source file for Sprite Loading functions.

#include <sprite.h>
#include <log.h>
#include <util.h>
#include <gameglobal.h>

//Unload SpriteSet
void
unload_spriteset (spriteset * givss)
{
  if (!givss)
    return;

//Remove Frames
  if (givss->frames)
    free (givss->frames);
  givss->frames = NULL;
  givss->nframes = 0;
  givss->type = SPRITESET_NULL;

#ifdef DEBUG
  log_printf ("Freed SpriteSet");
#endif //DEBUG
}

//Create SpriteSet
spriteset *
create_spriteset ()
{
  spriteset *newss = NULL;
  newss = malloc (sizeof (spriteset));

  if (!newss)
    return newss;

  newss->nframes = 0;
  newss->type = SPRITESET_NULL;
  newss->frames = NULL;

  return newss;
}

static const char sprite_type_table[SPRITESET_TYPES][MAXSTR] = {
  "WALK",
  "RUN",
  "SLOW",
  "STOP",
  "JUMP",
  "FLY",
  "CROUCH",
  "ATTACK",
  "HIT",
  "DEAD",
  "IDLE",
  "CLIMB",
  "DYING"
};

const char *
spriteset_type_to_str (int sst)
{
  if ((sst < 0) || (sst >= SPRITESET_TYPES))
    return NULL;
  return sprite_type_table[sst];
}

//Load SpriteSet
void
load_spriteset_file (FILE * fsprite, spriteset * givss)
{
  int i, allocsizeframes = 0, nframes = 0;
  char spritesettype[MAXSTR];

  if (!givss)
    return;

  fscanf (fsprite, "%s", spritesettype);

  fscanf (fsprite, "%d", &nframes);
  if (!nframes)
    return;
//givss->curframe=0;
  givss->nframes = nframes;
  givss->type = SPRITESET_NULL;

  for (i = 0; i < SPRITESET_TYPES; ++i)
    {
      if (!strcasecmp (sprite_type_table[i], spritesettype))
	{
	  givss->type = i;
	  break;
	}
    }
  if(givss->type == SPRITESET_NULL)
	return;

  givss->frames = NULL;

//Create Frames
  allocsizeframes = sizeof (spritesetframe) * (givss->nframes);
  givss->frames = malloc (allocsizeframes);

  if (!givss->frames)
    return;
  givss->nframes = nframes;

  memset (givss->frames, SPRITESET_NULL, allocsizeframes);

//Load Data
  for (i = 0; i < givss->nframes; ++i)
    {
      fscanf (fsprite, "%d%d%d%d",
	      &givss->frames[i].value, &givss->frames[i].extra_value,
	      &givss->frames[i].draw_mode, &givss->frames[i].coll_mode);
    }

#ifdef DEBUG
  log_printf ("Loaded SpriteSet %s with %d Frames",
	      sprite_type_table[givss->type], givss->nframes);
#endif //DEBUG
}

//Unload Sprite
void
unload_sprite (sprite * givsprite)
{
  int i;
  if (givsprite)
    {

      for (i = 0; i < SPRITESET_TYPES; ++i)
	{
	  if (givsprite->spritesets[i])
	    unload_spriteset (givsprite->spritesets[i]);
	  givsprite->spritesets[i] = NULL;
	}

      if (givsprite->spritebitmap)
	{
	  al_destroy_bitmap (givsprite->spritebitmap);
	  givsprite->spritebitmap = NULL;
	}

//Destroying Sprite
      free (givsprite);
#ifdef VERBOSE
      log_printf ("Destroyed Sprite");
#endif //VERBOSE
    }
}

//Create Sprite
sprite *
create_sprite ()
{
  int i;
  sprite *newsprite = NULL;
  newsprite = malloc (sizeof (sprite));
  if (newsprite)
    {
      newsprite->spritewidth = 0;
      newsprite->spriteheight = 0;
      newsprite->spritebitmap = NULL;
      if (SPRITESET_TYPES > 0)
	{
	  for (i = 0; i < SPRITESET_TYPES; ++i)
	    newsprite->spritesets[i] = NULL;
	}
    }
  return newsprite;
}

//Load Sprite from a pre opened file
sprite *
load_sprite_file (FILE * fsprite, char *spritename)
{
  int i;
  int nspritesets;
  char filename[MAXSTR], spritebitmapfilename[MAXSTR];
  sprite *newsprite = NULL;
  ALLEGRO_BITMAP *sprbitmap = NULL;

  *filename = 0;
  fscanf (fsprite, "%s", filename);
  if (!filename || !*filename)
    return newsprite;

  *spritebitmapfilename = 0;
  sprintf (spritebitmapfilename, "%s%c%s%c%s%c%s", DATADIR, PATHSEP,
	   "Sprites", PATHSEP, spritename, PATHSEP, filename);

//Create Empty Sprite
  newsprite = create_sprite ();

  if (!newsprite) 
    return newsprite;

//Dimensions
  fscanf (fsprite, "%d%d", &newsprite->spritewidth, &newsprite->spriteheight);
#ifdef VERBOSE
  log_printf ("Dimensions: %dx%d", newsprite->spritewidth,
	      newsprite->spriteheight);
#endif //VERBOSE

#ifdef VERBOSE
  log_printf ("Sprite BITMAP: %s", spritebitmapfilename);
#endif //VERBOSE

//Load Bitmap
  sprbitmap = load_bitmap_ex (spritebitmapfilename, NULL);
  if (!sprbitmap)
    return newsprite;

  newsprite->spritebitmap = sprbitmap;

  nspritesets = 0;
  fscanf (fsprite, "%d", &nspritesets);

//Sprite Sets
  for (i = 0; i < nspritesets; ++i)
    {
      spriteset *newspriteset = NULL;
#ifdef VERBOSE
      log_printf ("SpriteSet %d", i);
#endif //VERBOSE
      newspriteset = create_spriteset ();
      load_spriteset_file (fsprite, newspriteset);
      if (newspriteset->type != SPRITESET_NULL
	  && !newsprite->spritesets[newspriteset->type])
	{
	  newsprite->spritesets[newspriteset->type] = newspriteset;
	}
      else
	{
	  unload_spriteset (newspriteset);
	}
      newspriteset = NULL;
    }

  return newsprite;
}

//Load sprite from a file
sprite *
load_sprite (char *spritename)
{
  char spritefilename[MAXSTR];
  FILE *fsprite;
  sprite *newsprite = NULL;

  if (!*spritename)
    return newsprite;

  *spritefilename = 0;
//From: Data/Sprites/spritename/sprite.txt
  sprintf (spritefilename, "%s%c%s%c%s%c%s", DATADIR, PATHSEP, "Sprites",
	   PATHSEP, spritename, PATHSEP, "sprite.txt");

  fsprite = fopen (spritefilename, "r");
  if (!fsprite)
    return newsprite;

//Loading Sprite
#ifdef VERBOSE
  log_printf ("Loading Sprite from: %s", spritefilename);
#endif //VERBOSE

  newsprite = load_sprite_file (fsprite, spritename);

  fclose (fsprite);
  fsprite = NULL;

  return newsprite;
}
