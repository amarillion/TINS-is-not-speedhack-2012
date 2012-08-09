//Source file for sprite animation.

#include <global.h>
#include <sprite.h>
#include <log.h>

#define SPRITE_ANIM_LIMIT MSEC_TO_LPS(80)
static int sprite_anim_timer;

//Uninitialize Sprite Animater
void
quit_sprite_animater ()
{
  log_printf ("Quitting Sprite Animater");
}

//Initialize Sprite Animater
int
init_sprite_animater ()
{
  sprite_anim_timer = 0;
  log_printf ("Initializing Sprite Animater");
  return 0;
}

//Animate SpriteInstance
int
animate_spriteinstance (spriteinstance * givsi, int flags)
{
  int nframes = givsi->sprite->spritesets[givsi->cursst]->nframes;
  givsi->curframe++;
  if (givsi->curframe >= nframes)
    {
      givsi->curframe = 0;
    }
  if(nframes == 1)
    {
	return 0;
    }
  return 1;
}

//Animate Sprite
int
animate_sprite (spriteinstance * givsi, int flags)
{
  int sst = givsi->cursst;
  int status = 0;
  if (!givsi)
    return status;
  if (givsi->sprite->spritesets[sst])
    status += animate_spriteinstance (givsi, flags);
  return status;
}

//Change Sprite State
void
sprite_set_state (spriteinstance * givsi, int newsst)
{
  if (!givsi)
    return;
   givsi->prevsst = givsi->cursst;
   givsi->cursst = newsst;
   if (givsi->cursst != givsi->prevsst) {
     givsi->curframe = 0;
   }
  //Only reset animation if the state changed
   if(newsst == SPRITESET_DEAD)
	   log_printf("[SSS] %d",newsst);
}

//Retrieve Sprite State
int sprite_get_state (spriteinstance * givsi)
{
  if (!givsi)
    return SPRITESET_NULL;
  return givsi->cursst;
}

