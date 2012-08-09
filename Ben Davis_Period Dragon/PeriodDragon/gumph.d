module gumph;

import drawing;
import main;

import allegro5.allegro;
import allegro5.allegro_font;

void drawGumph() {
	setNormalBlender();

	ALLEGRO_COLOR col1=ALLEGRO_COLOR(1,1,1,1);
	ALLEGRO_COLOR col2=ALLEGRO_COLOR(0.5,0.5,0.5,1);

	al_draw_text(font,col1,SCREEN_W/2,SCREEN_H*4/30,ALLEGRO_ALIGN_CENTRE,"Period Dragon");
	al_draw_text(font,col2,SCREEN_W/2,SCREEN_H*6/30,ALLEGRO_ALIGN_CENTRE,"Written in two days for TINS 2012");
	al_draw_text(font,col2,SCREEN_W/2,SCREEN_H*7/30,ALLEGRO_ALIGN_CENTRE,"A weekend Allegro game programming competition");
	al_draw_text(font,col2,SCREEN_W/2,SCREEN_H*21/30,ALLEGRO_ALIGN_CENTRE,"Code/art/levels by me - font from 1001freefonts.com");
	al_draw_text(font,col2,SCREEN_W/2,SCREEN_H*23/30,ALLEGRO_ALIGN_CENTRE,"Written in D, the *real* successor to C.");
	al_draw_text(font,col2,SCREEN_W/2,SCREEN_H*24/30,ALLEGRO_ALIGN_CENTRE,"Special thanks to SiegeLord for making this possible!");
	al_draw_text(font,col2,SCREEN_W/2,SCREEN_H*26/30,ALLEGRO_ALIGN_CENTRE,`Copyright (D) 2012 Ben 'Bruce "entheh" Perry' Davis`);
}
