module main;

pragma(lib, "dallegro5");
pragma(lib, "allegro-505-monolith-mt-dll");

import allegro5.allegro;
import allegro5.allegro_primitives;
import allegro5.allegro_image;
import allegro5.allegro_font;
import allegro5.allegro_ttf;
import allegro5.allegro_color;

import std.stdio;
import std.math;
import std.conv;
import std.string;

import leveltypes;
import drawing;
import util;
import circuits;
import tilesizes;
import editing;
import torches;
import levelfiles;
import gaming;
import menus;
import gamefiles;
import gumph;

Level[] numberedLevels;
int maxStartingLevel;
ALLEGRO_DISPLAY* display;
ALLEGRO_FONT* font;
bool[ALLEGRO_KEY_MAX] keys;
bool exit = false;
bool isFullScreen = false;
ALLEGRO_CONFIG* cfg;

@property int SCREEN_W() {
	return al_get_display_width(display);
}

@property int SCREEN_H() {
	return al_get_display_height(display);
}

void main(char[][] args) {
	//The Windows debug build is a console app - make sure the console stays open if there's an error.
	debug {
		try {
			main2(args);
		}
		catch (Throwable t) {
			writeln(t.toString());
			writeln("Press Enter to close");
			stdin.readln();
		}
	}
	//The Windows release build is a Windows app.
	else {
		main2(args);
	}
}

void main2(char[][] args) {
	al_init();

	loadConfig();

	al_install_keyboard();
	//al_install_mouse();
	al_init_image_addon();
	al_init_font_addon();
	al_init_ttf_addon();
	al_init_primitives_addon();

	numberedLevels=loadNumberedLevels();

	initDisplay();

	ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();

	ALLEGRO_TIMER* timer=al_create_timer(1f/50);

	al_register_event_source(queue, al_get_display_event_source(display));
	al_register_event_source(queue, al_get_keyboard_event_source());
	//al_register_event_source(queue, al_get_mouse_event_source());
	al_register_event_source(queue, al_get_timer_event_source(timer));

//	auto color1 = al_color_hsl(0, 0, 0);
//	auto color2 = al_map_rgba_f(0.5, 0.25, 0.125, 1);
//	writefln("%s, %s, %s, %s", color1.r, color1.g, color2.b, color2.a);

	if (LEVEL_TO_EDIT !is null) {
		game=new Game();
		game.level=createLevel();
		game.editor=new Editor();
		game.hooks=new class GameHooks {
			void abort() {exit=true;}
			void finishLevel() {}
			void restartLevel() {}
			string getLevelMessage() {return LEVEL_TO_EDIT;}
		};
	}
	else {
		openMainMenu();
	}

	int lastUpdate=0, lastDraw=-1;
	al_start_timer(timer);
	while (!exit) {
		ALLEGRO_EVENT event;
		while (al_get_next_event(queue, &event)) {
			switch (event.type) {
				case ALLEGRO_EVENT_DISPLAY_CLOSE:
					if (game) saveGame();
					exit = true;
					break;
				case ALLEGRO_EVENT_TIMER:
					if (lastUpdate-lastDraw<10) {
						if (menu) updateMenu();
						else updateGameLogic();
						lastUpdate++;
					}
					break;
				case ALLEGRO_EVENT_KEY_DOWN: {
					keys[event.keyboard.keycode]=true;
					break;
				}
				case ALLEGRO_EVENT_KEY_CHAR: {
					switch (event.keyboard.keycode) {
						case ALLEGRO_KEY_ESCAPE:
							if (menu) {
								if (game) closeMenu();
								else exit=true;
							}
							else if (game.editor) game.hooks.abort();
							else openInGameMenu();
							break;
						default:
					}
					if (menu) menuKeyChar(event);
					else if (game.editor) editorKeyChar(event);
					else gameKeyChar(event);
					break;
				}
				case ALLEGRO_EVENT_KEY_UP:
					keys[event.keyboard.keycode]=false;
					break;
				case ALLEGRO_EVENT_DISPLAY_SWITCH_OUT:
					keys[]=false;
					break;
//				case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN: {
//					exit = true;
//					break;
//				}
				default:
			}
		}
		if (lastUpdate-lastDraw>0) {
			drawAll();
			lastDraw=lastUpdate;
		}
		else al_rest(0.001);
	}

	saveConfig();
}

void updateGameLogic() {
	int maxSpeed(int turboTime) {
		int speed=turboTime*MAXSPEED_TURBO/TIME_TO_MAXSPEED_TURBO;
		if (speed<MAXSPEED_BASIC) speed=MAXSPEED_BASIC;
		return speed;
	}

	int targetdx=0;
	if (keys[ALLEGRO_KEY_LEFT] || game.level.slidex<0) targetdx-=maxSpeed(game.level.turboTimeL);
	if (keys[ALLEGRO_KEY_RIGHT] || game.level.slidex>0) targetdx+=maxSpeed(game.level.turboTimeR);
	game.level.slidex=0;
	game.level.scrolldx+=sgn(targetdx-game.level.scrolldx);
	bool xCouldMove=tryMove(game.level.scrollx, game.level.scrolldx, FINE*game.level.xPeriod(), game.level.scrolly, game.level.scrolldy, FINE*game.level.yPeriod(), game.level.slidey);

	int targetdy=0;
	if (keys[ALLEGRO_KEY_UP] || game.level.slidey<0) targetdy-=maxSpeed(game.level.turboTimeU);
	if (keys[ALLEGRO_KEY_DOWN] || game.level.slidey>0) targetdy+=maxSpeed(game.level.turboTimeD);
	game.level.slidey=0;
	game.level.scrolldy+=sgn(targetdy-game.level.scrolldy);
	bool yCouldMove=tryMove(game.level.scrolly, game.level.scrolldy, FINE*game.level.yPeriod(), game.level.scrollx, game.level.scrolldx, FINE*game.level.xPeriod(), game.level.slidex);

	if (game.level.scrolldx<0) {if (game.level.turboTimeL<TIME_TO_MAXSPEED_TURBO) game.level.turboTimeL++;} else {game.level.turboTimeL-=5; if (game.level.turboTimeL<0) game.level.turboTimeL=0;}
	if (game.level.scrolldx>0) {if (game.level.turboTimeR<TIME_TO_MAXSPEED_TURBO) game.level.turboTimeR++;} else {game.level.turboTimeR-=5; if (game.level.turboTimeR<0) game.level.turboTimeR=0;}
	if (game.level.scrolldy<0) {if (game.level.turboTimeU<TIME_TO_MAXSPEED_TURBO) game.level.turboTimeU++;} else {game.level.turboTimeU-=5; if (game.level.turboTimeU<0) game.level.turboTimeU=0;}
	if (game.level.scrolldy>0) {if (game.level.turboTimeD<TIME_TO_MAXSPEED_TURBO) game.level.turboTimeD++;} else {game.level.turboTimeD-=5; if (game.level.turboTimeD<0) game.level.turboTimeD=0;}

	//What angle are we facing?
	game.level.breathingFire=false;
	if (game.level.scrolldx!=0 || game.level.scrolldy!=0) {
		float targetAngle=atan2(cast(float)game.level.scrolldx,-cast(float)game.level.scrolldy);
		//Take the shorter route...
		float diff=targetAngle-game.level.faceAngle;
		while (diff<-PI) diff+=2*PI;
		while (diff>+PI) diff-=2*PI;
		//Turn harder the faster we're going.
		float speed=fmax(abs(game.level.scrolldx),abs(game.level.scrolldy));
		if (speed>MAXSPEED_BASIC) speed=MAXSPEED_BASIC;
		game.level.faceAngle+=diff*speed/MAXSPEED_BASIC;
		game.level.faceAngle%=2*PI;
	}
	else {
		bool l=keys[ALLEGRO_KEY_LEFT];
		bool r=keys[ALLEGRO_KEY_RIGHT];
		bool u=keys[ALLEGRO_KEY_UP];
		bool d=keys[ALLEGRO_KEY_DOWN];
		if (!yCouldMove && !l && !r && u && !d) breatheFire(0);
		if (!xCouldMove && !l && r && !u && !d) breatheFire(PI/2);
		if (!yCouldMove && !l && !r && !u && d) breatheFire(PI);
		if (!xCouldMove && l && !r && !u && !d) breatheFire(PI*3/2);
		//Diagonals...
		if (!xCouldMove && !yCouldMove) {
			if (!l && r && u && !d) breatheFire(PI/4);
			if (!l && r && !u && d) breatheFire(PI*3/4);
			if (l && !r && !u && d) breatheFire(PI*5/4);
			if (l && !r && u && !d) breatheFire(PI*7/4);
		}
	}

	if (game.editor) runEditorControls();

	//Collect stuff
	checkCurrentPosition(true);

	burnStuffUsingTorches();
	updatePoweredCircuits();

	updateStats(false);
}

//Tries to update u (x or y) by du. If a collision happens, u is moved as far as possible, and du is set to 0.
//Returns false if the move was blocked, true otherwise (true if no move wanted, true if sliding).
bool tryMove(ref int u, ref int du, int uPeriod, ref int v, ref int dv, int vPeriod, ref int slidev) {
	bool couldMove=true;
	bool ok1=checkCurrentPosition(false);
	int oldu=u;
	u=saneMod(u+du, uPeriod);
	bool ok2=checkCurrentPosition(false);
	if (ok1 && !ok2) {
		//First, see if sliding along the other axis would help.
		int oldv=v;
		int newv=((v+FINE/2)/FINE)*FINE;
		v=saneMod(newv, vPeriod);
		bool okAfterSliding=checkCurrentPosition(false);
		v=oldv;
		if (okAfterSliding) {
			int needToSlide=abs(newv-oldv);
			int estimatedSlideIfWeSetSlidev=((abs(dv)+1)*(abs(dv)+2))/2;
			if (estimatedSlideIfWeSetSlidev<=needToSlide) slidev=sgn(newv-oldv);
		}
		else couldMove=false;
		//Now move as far as we can in the original axis without colliding.
		int newu=u;
		u=oldu;
		du=sgn(du);
		while (u!=newu) {
			u=saneMod(u+du, uPeriod);
			if (checkCurrentPosition(false)) continue;
			u=saneMod(u-du, uPeriod);
			du=0;
			break;
		}
	}
	return couldMove;
}

//Determines whether the player is OK at the current coordinates.
bool checkCurrentPosition(bool collect) {
	if (game.editor) return true;
	int playerSize=(collect ? PLAYER_SIZE_FOR_COLLECTION : PLAYER_SIZE_FOR_COLLISIONS);
	foreach (Plane plane; game.level.planes) {
		//Check FINE is even. If it's not, this'll fail when planes have even dimensions (although they probably won't for this game).
		static assert (FINE%2==0);
		//The player is initially at the very centre tile of each plane. This is offset by the current scroll position.
		int playerxFine=saneMod((plane.w-1)*FINE/2 + game.level.scrollx, plane.w*FINE);
		int playeryFine=saneMod((plane.h-1)*FINE/2 + game.level.scrolly, plane.h*FINE);
		//Here's a function to help us check the player's position in the current plane.
		bool checkPosition(int x, int y) {
			x=saneMod(x, plane.w);
			y=saneMod(y, plane.h);
			TILE* tile=&plane.map[y][x];
			if (tile.type==TileType.CLEAR) return true;
			if (tile.type==TileType.DIAMOND) {
				if (collect) tile.type=TileType.CLEAR;
				return true;
			}
			if (tile.type==TileType.TRACK && !tile.track.terminal) return true;
			return false;
		}
		//Check all the tiles the player is overlapping with (up to four of them).
		assert ((FINE-playerSize)%2==0);
		int x1=(playerxFine+(FINE-playerSize)/2)/FINE;
		int x2=(playerxFine+(FINE+playerSize)/2-1)/FINE;
		int y1=(playeryFine+(FINE-playerSize)/2)/FINE;
		int y2=(playeryFine+(FINE+playerSize)/2-1)/FINE;
		if (!checkPosition(x1, y1)) return false;
		if (!checkPosition(x2, y1)) return false;
		if (!checkPosition(x1, y2)) return false;
		if (!checkPosition(x2, y2)) return false;
	}
	return true;
}

void breatheFire(float targetAngle) {
	//Take the shorter route...
	float diff=targetAngle-game.level.faceAngle;
	while (diff<-PI) diff+=2*PI;
	while (diff>+PI) diff-=2*PI;
	game.level.faceAngle+=diff/2;
	game.level.faceAngle%=2*PI;
	game.level.breathingFire=true;
}

void drawAll() {
	al_clear_to_color(ALLEGRO_COLOR(0, 0, 0, 1));
	//al_clear_to_color(ALLEGRO_COLOR(0.5, 0.25, 0.125, 1));
	//al_draw_bitmap(bmp, 50, 50, 0);
	//al_draw_triangle(20, 20, 300, 30, 200, 200, ALLEGRO_COLOR(1, 1, 1, 1), 4);
	//al_draw_text(font, ALLEGRO_COLOR(1, 1, 1, 1), 70, 40, ALLEGRO_ALIGN_CENTRE, "Hello!");

	if (game) drawGame();
	else drawGumph();

	if (menu) drawMenu();

	al_flip_display();
}

void drawGame() {
	draw(game);
}

void toggleFullScreen() {
	isFullScreen=!isFullScreen;
	initDisplay();
}

void initDisplay() {
	destroyBitmaps();
	if (font) al_destroy_font(font);
	if (display) al_destroy_display(display);

	if (isFullScreen) {
		al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
		display = al_create_display(0, 0);
	}
	else {
		al_set_new_display_flags(ALLEGRO_WINDOWED);
		display = al_create_display(1920*4/10, 1200*4/10);
	}

	al_set_window_title(display,"Period Dragon");

	font = al_load_font("Flames.ttf", -SCREEN_H/30, 0);
	loadBitmaps();
}

void loadConfig() {
	if (!cfg) {
		cfg = al_load_config_file("PeriodDragon.ini");
		if (!cfg) cfg = al_create_config();
	}

	const(char)* cstr=al_get_config_value(cfg, "game", "fullscreen");
	if (cstr) {
		string str=to!string(cstr);
		isFullScreen=(str.length>0 && str!="0" && str!="false");
	}

	cstr=al_get_config_value(cfg, "game", "maxStartingLevel");
	if (cstr) {
		string str=to!string(cstr);
		maxStartingLevel=to!int(str);
	}
}

void saveConfig() {
	al_set_config_value(cfg, "game", "fullscreen", isFullScreen?"1":"0");
	al_set_config_value(cfg, "game", "maxStartingLevel", toStringz(to!string(maxStartingLevel)));
	al_save_config_file("PeriodDragon.ini", cfg);
}
