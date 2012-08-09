module menus;

import allegro5.allegro;
import allegro5.allegro_font;
import allegro5.allegro_primitives;

import gaming;
import gamefiles;
import main;
import util;
import drawing;

import std.string;

struct MENU_ITEM {
	string delegate() text;
	void delegate() select;
	void delegate() left,right;
}

MENU_ITEM[] menu;
int menuOpt;
int startingLevel;

void openMainMenu() {
	menu=[
		getStartGame(),
	];
	if (saveGameExists()) menu~=getLoadGame();
	menu~=[
		getFullScreen(),
		getExit(),
	];
	menuOpt=0;
	startingLevel=maxStartingLevel;
	if (startingLevel<0 || startingLevel>=numberedLevels.length) startingLevel=0;
}

void openInGameMenu() {
	menu=[
		getSaveGame(),
	];
	if (saveGameExists()) menu~=getLoadGame();
	menu~=[
		getRestartLevel(),
	];
	if (maxStartingLevel>0) menu~=getGoToLevel();
	menu~=[
		getRestartGame(),
		getFullScreen(),
		getSaveAndExit(),
		getExitWithoutSaving(),
	];
	menuOpt=0;
	startingLevel=game.levelNum;
}



void updateMenu() {}

void closeMenu() {
	menu=null;
}

void drawMenu() {
	//Fonts ARE premultiplied by alpha.
	setNormalBlender();

	int longest=0;
	foreach (i, ref item; menu) {
		int length=al_get_text_width(font,toStringz(item.text()));
		if (length>longest) longest=length;
	}

	int bx=longest/2+SCREEN_W/40, by=SCREEN_H*(menu.length+2)/60;
	int rx=SCREEN_W/40, ry=SCREEN_H/30;
	al_draw_filled_rounded_rectangle(SCREEN_W/2-bx,SCREEN_H/2-by,SCREEN_W/2+bx,SCREEN_H/2+by,rx,ry,ALLEGRO_COLOR(0,0,0,0.5));

	int y=30-menu.length;
	foreach (i, ref item; menu) {
		ALLEGRO_COLOR col=ALLEGRO_COLOR(0.6,0.8,0.3,1);
		if (menuOpt==i) col=ALLEGRO_COLOR(1,1,1,1);
		al_draw_text(font,col,SCREEN_W/2,SCREEN_H*y/60,ALLEGRO_ALIGN_CENTRE,toStringz(item.text()));
		y+=2;
	}
}

void menuKeyChar(ref ALLEGRO_EVENT event) {
	switch (event.keyboard.keycode) {
		case ALLEGRO_KEY_UP:
			menuOpt=saneMod(menuOpt-1,menu.length);
			break;
		case ALLEGRO_KEY_DOWN:
			menuOpt=saneMod(menuOpt+1,menu.length);
			break;
		case ALLEGRO_KEY_LEFT:
			if (menu[menuOpt].left) menu[menuOpt].left();
			break;
		case ALLEGRO_KEY_RIGHT:
			if (menu[menuOpt].right) menu[menuOpt].right();
			break;
		case ALLEGRO_KEY_ENTER, ALLEGRO_KEY_SPACE:
			menu[menuOpt].select();
			break;
		default:
	}
}



private MENU_ITEM getStartGame() {
	return MENU_ITEM(() => maxStartingLevel>0 ? format("Start at level %s",startingLevel+1) : "Start game",
					 () { startGame(startingLevel); closeMenu(); },
					 () { if (startingLevel>0) startingLevel--; },
					 () { if (startingLevel<maxStartingLevel && startingLevel<numberedLevels.length-1) startingLevel++; });
}

private MENU_ITEM getGoToLevel() {
	return MENU_ITEM(() => format("Go to level %s",startingLevel+1),
					 () { startGame(startingLevel); closeMenu(); },
					 () { if (startingLevel>0) startingLevel--; },
					 () { if (startingLevel<maxStartingLevel && startingLevel<numberedLevels.length-1) startingLevel++; });
}

private MENU_ITEM getSaveGame() {
	return MENU_ITEM(() => "Save game",
					 () { saveGame(); closeMenu(); });
}

private MENU_ITEM getLoadGame() {
	return MENU_ITEM(() => "Load game",
					 () { loadGame(); closeMenu(); });
}

private MENU_ITEM getFullScreen() {
	return MENU_ITEM(() => isFullScreen ? "Go windowed" : "Go full screen",
					 () { toggleFullScreen(); });
}

private MENU_ITEM getExit() {
	return MENU_ITEM(() => "Exit",
					 () { exit=true; });
}

private MENU_ITEM getSaveAndExit() {
	return MENU_ITEM(() => "Save and exit",
					 () { saveGame(); exit=true; });
}

private MENU_ITEM getExitWithoutSaving() {
	return MENU_ITEM(() => "Exit without saving",
					 () { exit=true; });
}

private MENU_ITEM getRestartLevel() {
	return MENU_ITEM(() => "Restart level",
					 () { startLevel(game.levelNum); closeMenu(); });
}

private MENU_ITEM getRestartGame() {
	return MENU_ITEM(() => "Restart game",
					 () { startLevel(0); closeMenu(); });
}
