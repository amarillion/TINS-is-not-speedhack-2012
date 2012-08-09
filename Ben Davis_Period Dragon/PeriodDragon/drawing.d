module drawing;

import allegro5.allegro;
import allegro5.allegro_primitives;
import allegro5.allegro_font;

import leveltypes;
import main;
import util;
import tilesizes;
import gaming;

import std.math;
import std.string;

ALLEGRO_BITMAP* bmpBricks;
ALLEGRO_BITMAP* bmpTerminal;
ALLEGRO_BITMAP* bmpTerminalBurnt;
ALLEGRO_BITMAP* bmpDiamond;
ALLEGRO_BITMAP* bmpTorch;
ALLEGRO_BITMAP* bmpTorchLit;
ALLEGRO_BITMAP* bmpCoal;
ALLEGRO_BITMAP* bmpFire;
ALLEGRO_BITMAP* bmpDragon;

void destroyBitmaps() {
	al_destroy_bitmap(bmpBricks);
	al_destroy_bitmap(bmpTerminal);
	al_destroy_bitmap(bmpTerminalBurnt);
	al_destroy_bitmap(bmpDiamond);
	al_destroy_bitmap(bmpTorch);
	al_destroy_bitmap(bmpTorchLit);
	al_destroy_bitmap(bmpCoal);
	al_destroy_bitmap(bmpFire);
	al_destroy_bitmap(bmpDragon);
}

void loadBitmaps() {
	al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP | ALLEGRO_MIPMAP | ALLEGRO_MIN_LINEAR);
	bmpBricks=al_load_bitmap("gfx/bricks.jpg");
	bmpTerminal=al_load_bitmap("gfx/terminal.jpg");
	bmpTerminalBurnt=al_load_bitmap("gfx/terminalburnt.jpg");
	bmpDiamond=al_load_bitmap("gfx/diamond.png");
	bmpTorch=al_load_bitmap("gfx/torch.png");
	bmpTorchLit=al_load_bitmap("gfx/torchlit.png");
	bmpCoal=al_load_bitmap("gfx/coal.png");
	bmpFire=al_load_bitmap("gfx/fire.png");
	bmpDragon=al_load_bitmap("gfx/dragon.png");
}

void setNormalBlender() {
	with (ALLEGRO_BLEND_MODE) al_set_blender(ALLEGRO_BLEND_OPERATIONS.ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA);
}

void setNormalBlenderNoPremulAlpha() {
	with (ALLEGRO_BLEND_MODE) al_set_blender(ALLEGRO_BLEND_OPERATIONS.ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
}

void setAdditiveBlender() {
	with (ALLEGRO_BLEND_MODE) al_set_blender(ALLEGRO_BLEND_OPERATIONS.ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ONE);
}

void setAdditiveBlenderNoPremulAlpha() {
	with (ALLEGRO_BLEND_MODE) al_set_blender(ALLEGRO_BLEND_OPERATIONS.ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_ONE);
}

void draw(Game game) {
	setNormalBlender();
	//setNormalBlenderNoPremulAlpha();

	foreach_reverse (planeNum, plane; game.level.planes) {
		float darken=1-pow(0.6,cast(float)planeNum/game.level.planes.length);

		//Old colours from when it was all rectangle primitives
		//ALLEGRO_COLOR wallCol=ALLEGRO_COLOR(1-darken,0.9-darken,0.7-darken,1);
		//ALLEGRO_COLOR diamondCol=ALLEGRO_COLOR(0.5-darken,0.8-darken,1-darken,0.8);
		//ALLEGRO_COLOR generatorCol=ALLEGRO_COLOR(0.5-darken,0.8-darken,1-darken,0.2);
		//ALLEGRO_COLOR trackUnpoweredCol=ALLEGRO_COLOR(1-darken,0.7-darken,0.5-darken,0.5);
		//ALLEGRO_COLOR trackPoweredCol=ALLEGRO_COLOR(0.5-darken,0,1.0-darken,0.5);
		//ALLEGRO_COLOR terminalUnpoweredCol=ALLEGRO_COLOR(1-darken,0.8-darken,0.5-darken,1);
		//ALLEGRO_COLOR terminalPoweredCol=ALLEGRO_COLOR(0.6-darken,0.5-darken,1-darken,1);
		ALLEGRO_COLOR torchUnlitCol=ALLEGRO_COLOR(0.8-darken,0.7-darken,0.6-darken,1);
		ALLEGRO_COLOR torchLitCol=ALLEGRO_COLOR(1-darken,1-darken,0.9-darken,1);
		ALLEGRO_COLOR coalCol=ALLEGRO_COLOR(0.5-darken,0.5-darken,0.5-darken,1);
		ALLEGRO_COLOR playerCol=ALLEGRO_COLOR(0.5-darken,1-darken,0.5-darken,0.6);

		//New colours for manipulating bitmaps with
		ALLEGRO_COLOR premul(float r, float g, float b, float a) {
			return ALLEGRO_COLOR(r*a,g*a,b*a,a);
		}
		ALLEGRO_COLOR generalTint=premul(1-darken*2,1-darken*2,1-darken*2,1);
		ALLEGRO_COLOR diamondTint1=premul(1-darken*2,1-darken*2,1-darken*2,0.8);
		ALLEGRO_COLOR diamondTint2=premul(1-darken*2,1-darken*2,1-darken*2,0.2);
		ALLEGRO_COLOR generatorTint=premul(1-darken*2,1-darken*2,1-darken*2,0.2);
		ALLEGRO_COLOR terminalTint=premul(1-darken*2,1-darken*2,1-darken*2,1);
		ALLEGRO_COLOR trackTint=premul(1-darken*2,1-darken*2,1-darken*2,0.5);
		ALLEGRO_COLOR terminalPowerCol=premul(0,0.2,1-darken,1);
		ALLEGRO_COLOR trackPowerCol=premul(0,0.2,1-darken,0.9);

		foreach (y; 0..plane.h) {
			foreach (x; 0..plane.w) {
				TILE *tile=&plane.map[y][x];

				//I love D :)
				void drawTile(int scrx, int scry) {
					float x1=scrx*SCREEN_W/cast(float)(FINE*plane.w);
					float x2=(scrx+FINE)*SCREEN_W/cast(float)(FINE*plane.w);
					float y1=scry*SCREEN_H/cast(float)(FINE*plane.h);
					float y2=(scry+FINE)*SCREEN_H/cast(float)(FINE*plane.h);
					//I love D too :)
					auto x = (float f) => x1+(x2-x1)*f;
					auto y = (float f) => y1+(y2-y1)*f;
					auto w = (float f) => (x2-x1)*f;
					auto h = (float f) => (y2-y1)*f;
					switch (tile.type) {
						case TileType.CLEAR:
							break;
						case TileType.DIAMOND:
							//al_draw_filled_rectangle(x(0.2),y(0.2),x(0.8),y(0.8),diamondCol);
							//Draw the diamond at 0.8 opacity...
							al_draw_tinted_scaled_bitmap(bmpDiamond,diamondTint1,0,0,al_get_bitmap_width(bmpDiamond),al_get_bitmap_height(bmpDiamond),x1,y1,x2-x1,y2-y1,0);
							//Then draw it again with the remaining 0.2, additively. This means the diamond is translucent but doesn't lose any brightness.
							setAdditiveBlender();
							al_draw_tinted_scaled_bitmap(bmpDiamond,diamondTint2,0,0,al_get_bitmap_width(bmpDiamond),al_get_bitmap_height(bmpDiamond),x1,y1,x2-x1,y2-y1,0);
							setNormalBlender();
							break;
						case TileType.WALL:
							al_draw_tinted_scaled_bitmap(bmpBricks,generalTint,0,0,al_get_bitmap_width(bmpBricks),al_get_bitmap_height(bmpBricks),x1,y1,x2-x1,y2-y1,0);
							//al_draw_filled_rectangle(x1,y1,x2,y2,wallCol);
							break;
						case TileType.TRACK:
							//ALLEGRO_COLOR* trackCol=(tile.track.powered ? &trackPoweredCol : &trackUnpoweredCol);
							//ALLEGRO_COLOR* terminalCol=(tile.track.powered ? &terminalPoweredCol : &terminalUnpoweredCol);
							//ALLEGRO_COLOR* terminalTint=(tile.track.powered ? &terminalPoweredTint : &terminalUnpoweredTint);
							/*
							float burntAdjust=(tile.track.burnt ? 0.4 : 0);
							if (tile.track.l) al_draw_filled_rectangle(x1,y(0.4),x(0.5-burntAdjust),y(0.6),*trackCol);
							if (tile.track.r) al_draw_filled_rectangle(x(0.5+burntAdjust),y(0.4),x2,y(0.6),*trackCol);
							if (tile.track.u) al_draw_filled_rectangle(x(0.4),y1,x(0.6),y(0.5-burntAdjust),*trackCol);
							if (tile.track.d) al_draw_filled_rectangle(x(0.4),y(0.5+burntAdjust),x(0.6),y2,*trackCol);
							*/
							ALLEGRO_BITMAP* bmp=(tile.track.burnt ? bmpTerminalBurnt : bmpTerminal);
							int tw=al_get_bitmap_width(bmp);
							int th=al_get_bitmap_height(bmp);
							al_draw_tinted_scaled_bitmap(bmp,trackTint,tw*0.4,th*0.4,tw*0.2,th*0.2,x(0.4),y(0.4),w(0.2),h(0.2),0);
							if (tile.track.l) al_draw_tinted_scaled_bitmap(bmp,trackTint,0,th*0.4,tw*0.4,th*0.2,x1,y(0.4),w(0.4),h(0.2),0);
							if (tile.track.r) al_draw_tinted_scaled_bitmap(bmp,trackTint,tw*0.6,th*0.4,tw*0.4,th*0.2,x(0.6),y(0.4),w(0.4),h(0.2),0);
							if (tile.track.u) al_draw_tinted_scaled_bitmap(bmp,trackTint,tw*0.4,0,tw*0.2,th*0.4,x(0.4),y1,w(0.2),h(0.4),0);
							if (tile.track.d) al_draw_tinted_scaled_bitmap(bmp,trackTint,tw*0.4,th*0.6,tw*0.2,th*0.4,x(0.4),y(0.6),w(0.2),h(0.4),0);
							if (tile.track.powered) {
								setAdditiveBlender();
								al_draw_filled_rectangle(x(0.4),y(0.4),x(0.6),y(0.6),trackPowerCol);
								if (tile.track.l) al_draw_filled_rectangle(x1,y(0.4),x(0.4),y(0.6),trackPowerCol);
								if (tile.track.r) al_draw_filled_rectangle(x(0.6),y(0.4),x2,y(0.6),trackPowerCol);
								if (tile.track.u) al_draw_filled_rectangle(x(0.4),y1,x(0.6),y(0.4),trackPowerCol);
								if (tile.track.d) al_draw_filled_rectangle(x(0.4),y(0.6),x(0.6),y2,trackPowerCol);
								setNormalBlender();
							}

							if (tile.track.terminal) {
								float f1=(FINE-TERMINAL_SIZE)/cast(float)(2*FINE);
								float f2=(FINE+TERMINAL_SIZE)/cast(float)(2*FINE);
								al_draw_tinted_scaled_bitmap(bmp,terminalTint,tw*f1,th*f1,tw*(f2-f1),th*(f2-f1),x(f1),y(f1),w(f2-f1),h(f2-f1),0);
								//al_draw_filled_rectangle(x(f1),y(f1),x(f2),y(f2),*terminalCol);
								if (tile.track.powered) {
									setAdditiveBlender();
									al_draw_filled_rectangle(x(f1),y(f1),x(f2),y(f2),terminalPowerCol);
									setNormalBlender();
								}
							}
							break;
						case TileType.GENERATOR:
							al_draw_tinted_scaled_bitmap(bmpDiamond,generatorTint,0,0,al_get_bitmap_width(bmpDiamond),al_get_bitmap_height(bmpDiamond),x1,y1,x2-x1,y2-y1,0);
							//al_draw_filled_rectangle(x(0.2),y(0.2),x(0.8),y(0.8),generatorCol);
							break;
						case TileType.TORCH:
							al_draw_tinted_scaled_bitmap(tile.torch.lit ? bmpTorchLit : bmpTorch,generalTint,0,0,al_get_bitmap_width(bmpTorch),al_get_bitmap_height(bmpTorch),x1,y1,x2-x1,y2-y1,0);
							//al_draw_filled_rectangle(x(0.3),y(0.3),x(0.7),y(0.7), tile.torch.lit ? torchLitCol : torchUnlitCol);
							break;
						case TileType.COAL:
							al_draw_tinted_scaled_bitmap(bmpCoal,generalTint,0,0,al_get_bitmap_width(bmpCoal),al_get_bitmap_height(bmpCoal),x1,y1,x2-x1,y2-y1,0);
							//al_draw_filled_rectangle(x(0.3),y(0.3),x(0.7),y(0.7), coalCol);
							break;
						default:
							throw new Exception("Unsupported tile: "~tile.type);
					}
				}

				int scrx=saneMod(x*FINE-game.level.scrollx, plane.w*FINE);
				int scry=saneMod(y*FINE-game.level.scrolly, plane.h*FINE);
				drawTile(scrx,scry);
				if (scrx>(plane.w-1)*FINE) drawTile(scrx-plane.w*FINE, scry);
				if (scry>(plane.h-1)*FINE) {
					drawTile(scrx, scry-plane.h*FINE);
					if (scrx>(plane.w-1)*FINE) drawTile(scrx-plane.w*FINE, scry-plane.h*FINE);
				}
			}
		}

		//In editor mode, scale the dragon to the plane being edited.
		if ((game.editor ? game.editor.planeNum : 0)==planeNum) {
			//Draw the player in the middle! (Use the correct tile size for this plane.)
			float pw=(SCREEN_W*PLAYER_SIZE_FOR_DRAWING)/cast(float)(plane.w*FINE);
			float ph=(SCREEN_H*PLAYER_SIZE_FOR_DRAWING)/cast(float)(plane.h*FINE);
			//This is approximate - ideally we want to scale AFTER rotating, but this function scales BEFORE!
			float s=sin(game.level.faceAngle), c=cos(game.level.faceAngle);
			float dw=al_get_bitmap_width(bmpDragon), dh=al_get_bitmap_height(bmpDragon);
			float xscale=(pw*c*c+ph*s*s)/dw;
			float yscale=(pw*s*s+ph*c*c)/dh;
			al_draw_tinted_scaled_rotated_bitmap(bmpDragon,generalTint,dw/2f,dh/2f,SCREEN_W/2f,SCREEN_H/2f,xscale,yscale,game.level.faceAngle,0);
			/*
			al_draw_filled_ellipse(SCREEN_W/2f, SCREEN_H/2f, pw/2f, ph/2f, playerCol);
			al_draw_line(SCREEN_W/2f, SCREEN_H/2f, SCREEN_W/2f+s*pw/2f, SCREEN_H/2f-c*pw/2f, playerCol, 1);
			*/
			//If breathing fire, draw fire!
			if (game.level.breathingFire) {
				//This is approximate - ideally we want to scale AFTER rotating, but this function scales BEFORE!
				float fw=al_get_bitmap_width(bmpFire), fh=al_get_bitmap_height(bmpFire);
				const float fireLength=1.2;
				const float fireCentreDist=0.5+fireLength/2;
				xscale=fireLength*(pw*c*c+ph*s*s)/fw;
				yscale=fireLength*(pw*s*s+ph*c*c)/fh;
				al_draw_tinted_scaled_rotated_bitmap(bmpFire,generalTint,fw/2f,fh/2f,SCREEN_W/2f+s*pw*fireCentreDist,SCREEN_H/2f-c*ph*fireCentreDist,xscale,yscale,game.level.faceAngle,0);
				/*
				al_draw_filled_triangle(
								 SCREEN_W/2f+s*pw/2f, SCREEN_H/2f-c*ph/2f,
								 SCREEN_W/2f+s*pw*1.6f-c*pw*0.3f, SCREEN_H/2f-c*ph*1.6f-s*ph*0.3f,
								 SCREEN_W/2f+s*pw*1.6f+c*pw*0.3f, SCREEN_H/2f-c*ph*1.6f+s*ph*0.3f,
								 torchLitCol);
				*/
			}
		}
	}

	//Draw some on-screen display stuff.
	//Fonts ARE premultiplied by alpha.
	with (ALLEGRO_BLEND_MODE) {
		al_set_blender(ALLEGRO_BLEND_OPERATIONS.ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA);
	}

	int y=1;
	if (!game.editor && game.generators.total>0) {
		al_draw_textf(font,ALLEGRO_COLOR(1,1,1,1),SCREEN_W*1/40,SCREEN_H*y++/30,ALLEGRO_ALIGN_LEFT,"Generators: %d/%d",
					  game.generators.total-game.generators.current,
					  game.generators.total);
	}
	if (!game.editor) {
		al_draw_textf(font,ALLEGRO_COLOR(1,1,1,1),SCREEN_W*1/40,SCREEN_H*y++/30,ALLEGRO_ALIGN_LEFT,"Diamonds: %d/%d",
					  game.diamonds.total+game.generators.total+game.coal.total-game.diamonds.current-game.generators.current-game.coal.current,
					  game.diamonds.total+game.generators.total+game.coal.total);
	}

	y=1;
	al_draw_textf(font,ALLEGRO_COLOR(1,1,1,1),SCREEN_W*39/40,SCREEN_H*y++/30,ALLEGRO_ALIGN_RIGHT,"Level: %s",toStringz(game.hooks.getLevelMessage()));
	if (!game.editor && game.torches.total>0) {
		al_draw_textf(font,ALLEGRO_COLOR(1,1,1,1),SCREEN_W*39/40,SCREEN_H*y++/30,ALLEGRO_ALIGN_RIGHT,"Torches: %d/%d",
					  game.torches.current,
					  game.torches.total);
	}
	if (!game.editor && game.coal.total>0) {
		al_draw_textf(font,ALLEGRO_COLOR(1,1,1,1),SCREEN_W*39/40,SCREEN_H*y++/30,ALLEGRO_ALIGN_RIGHT,"Coal: %d/%d",
					  game.coal.total-game.coal.current,
					  game.coal.total);
	}

	if (!game.editor && game.levelComplete) {
		if (game.levelNum==game.levels.length-1) {
			al_draw_text(font,ALLEGRO_COLOR(1,1,1,1),SCREEN_W/2,SCREEN_H*5/30,ALLEGRO_ALIGN_CENTRE,"Game complete!");
			al_draw_text(font,ALLEGRO_COLOR(1,1,1,1),SCREEN_W/2,SCREEN_H*7/30,ALLEGRO_ALIGN_CENTRE,"Thanks for playing!");
		}
		else {
			al_draw_text(font,ALLEGRO_COLOR(1,1,1,1),SCREEN_W/2,SCREEN_H*6/30,ALLEGRO_ALIGN_CENTRE,"Level complete!");
		}
	}
	if (!game.editor && game.levelLostMessage) {
		al_draw_text(font,ALLEGRO_COLOR(1,1,1,1),SCREEN_W/2,SCREEN_H*5/30,ALLEGRO_ALIGN_CENTRE,toStringz(game.levelLostMessage));
		al_draw_text(font,ALLEGRO_COLOR(1,1,1,1),SCREEN_W/2,SCREEN_H*7/30,ALLEGRO_ALIGN_CENTRE,"Press Backspace to restart.");
	}
}
