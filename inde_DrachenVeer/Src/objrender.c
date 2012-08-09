//Source file for level rendering.

#include <object.h>
#include <fonts.h>

#define HEALTHBAR_W 50
#define HEALTHBAR_H 5
#define HEALTHBAR_BG 0,0,0
#define HEALTHBAR_FG_LOW 255,0,0
#define HEALTHBAR_FG_MID 255,127,0
#define HEALTHBAR_FG_HIG 0,255,0

//Render Object.
void render_object (ALLEGRO_BITMAP * bitmap, object *obj, int stx, int sty, int flags){
   float x,y;
   float w,h; 
   float sposx,sposy;
   float scposx,scposy;
   float healthpct;
   ALLEGRO_COLOR healthcolor;
   int bwidth = al_get_bitmap_width (bitmap);
   int bheight = al_get_bitmap_height (bitmap);
   if(!obj || obj->garbage)
	return;
   w = obj->w;
   h = obj->h;
   x = obj->x - w/2.0f;
   y = obj->y - h/2.0f;
   scposx= obj->x - stx;
   scposy= obj->y - sty;
   sposx= scposx - w/2.0f;
   sposy= scposy - h/2.0f;
   if((sposx+w) < 0 || (sposy+h)<0 || sposx > bwidth || sposy > bheight)
	return;
   if (NULL != obj->sprinstance) 
	{ 
          int sprite_flags = (obj->facing == OBJECT_FACING_W) ? RENDER_SPRITE_FLIP_HORZ : 0;
	  render_sprite (bitmap, obj->sprinstance, scposx, scposy, obj->angle, sprite_flags);
	}
   else {
         if(flags & LEVRENDER_DEBUG_OBJECTS)
              al_draw_rectangle (sposx, sposy, sposx+w, sposy+h, al_map_rgb(0, 255, 255), 3.0f);
   }
   if(flags & LEVRENDER_DEBUG_OBJECTS)
	{
  	ALLEGRO_FONT *font = fm_get_font (FONT_UI);
        if (font)
          {
            char msg[MAXSTR];
            *msg = 0;
            sprintf(msg, "%4d, %4d", (int)x, (int)y);
            al_draw_text (font, al_map_rgb (255, 255, 255), sposx, sposy, ALLEGRO_ALIGN_LEFT, msg);
            *msg = 0; 
            sprintf(msg, "%dx%d", (int)w, (int)h);
            al_draw_text (font, al_map_rgb (255, 255, 0), sposx, sposy+h, ALLEGRO_ALIGN_LEFT, msg);
          } 
	}
   if(flags & LEVRENDER_HEALTHBARS) {
	   if(obj->type == OBJECT_PLAYER || obj->type == OBJECT_ENEMY) {
		   healthpct = obj->health / obj->health_max;
		   healthcolor = al_map_rgb(HEALTHBAR_FG_HIG);
		   if(healthpct < 0.33)
			   healthcolor = al_map_rgb(HEALTHBAR_FG_LOW);
		   else if(healthpct < 0.66)
			   healthcolor = al_map_rgb(HEALTHBAR_FG_MID);
		   healthpct *= HEALTHBAR_W;
		   al_draw_filled_rectangle(sposx + obj->w*0.5f - 0.5f*HEALTHBAR_W,sposy,sposx + obj->w*0.5f - 0.5f*HEALTHBAR_W + HEALTHBAR_W, sposy + HEALTHBAR_H, al_map_rgb(HEALTHBAR_BG));
		   al_draw_filled_rectangle(sposx + obj->w*0.5f - 0.5f*HEALTHBAR_W,sposy,sposx + obj->w*0.5f - 0.5f*HEALTHBAR_W + (int)healthpct, sposy + HEALTHBAR_H, healthcolor);
	   }
   }
}

