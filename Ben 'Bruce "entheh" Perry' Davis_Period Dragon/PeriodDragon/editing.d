module editing;

import allegro5.allegro;

import main;
import leveltypes;
import util;
import tilesizes;
import levelfiles;
import gaming;

const string LEVEL_TO_EDIT=null;//"level009.lvl";

class Editor {
	int planeNum;
	TRACK lastTrack;
	void delegate(TRACK *) setLastTrackDirAgain;

	this() {
		lastTrack.l=true;
		lastTrack.r=true;
	}
}

void runEditorControls() {
	//Check for keys that tell us to place stuff here.
	void check(int key, TileType type, void delegate(TILE* tile) initTile=null) {
		if (keys[key]) {
			TILE* tile=getCurrentTile();
			if (tile.type!=type) {
				*tile=TILE(type);
				if (initTile) initTile(tile);
			}
		}
	}
	check(ALLEGRO_KEY_1, TileType.CLEAR);
	check(ALLEGRO_KEY_2, TileType.DIAMOND);
	check(ALLEGRO_KEY_3, TileType.GENERATOR);
	check(ALLEGRO_KEY_4, TileType.TORCH);
	check(ALLEGRO_KEY_5, TileType.COAL);
	check(ALLEGRO_KEY_6, TileType.WALL);
}

void editorKeyChar(ref ALLEGRO_EVENT event) {
	switch (event.keyboard.keycode) {
		case ALLEGRO_KEY_PGUP:
			if (game.editor.planeNum<game.level.planes.length-1) game.editor.planeNum++;
			break;
		case ALLEGRO_KEY_PGDN:
			if (game.editor.planeNum>0) game.editor.planeNum--;
			break;
		case ALLEGRO_KEY_A:
			changeTrack(delegate (track) {track.l=true;});
			break;
		case ALLEGRO_KEY_D:
			changeTrack(delegate (track) {track.r=true;});
			break;
		case ALLEGRO_KEY_W:
			changeTrack(delegate (track) {track.u=true;});
			break;
		case ALLEGRO_KEY_S:
			if (event.keyboard.modifiers & ALLEGRO_KEYMOD_CTRL) {
				saveLevel(game.level,LEVEL_TO_EDIT);
			}
			else changeTrack(delegate (track) {track.d=true;});
			break;
		case ALLEGRO_KEY_L:
			if (event.keyboard.modifiers & ALLEGRO_KEYMOD_CTRL) {
				game.level=loadLevel(LEVEL_TO_EDIT);
				if (game.editor.planeNum>=game.level.planes.length) game.editor.planeNum=game.level.planes.length-1;
			}
			break;
		case ALLEGRO_KEY_T:
			{
				TILE *tile=getCurrentTile();
				if (tile.type==TileType.TRACK) tile.track.terminal=!tile.track.terminal;
			}
			break;
		case ALLEGRO_KEY_TAB:
			//Test the level!
			Game oldGame=game;
			game=new Game();
			startLevel(oldGame.level);
			game.hooks=new class GameHooks {
				void abort() {game=oldGame;}
				void finishLevel() {game=oldGame;}
				void restartLevel() {startLevel(oldGame.level);}
				string getLevelMessage() {return LEVEL_TO_EDIT;}
			};
			break;
		default:
	}
}

void changeTrack(void delegate(TRACK *) setTrackDir) {
	TILE* tile=getCurrentTile();
	if (tile.type!=TileType.TRACK) {
		*tile=TILE(TileType.TRACK);
		tile.track=game.editor.lastTrack;
	}
	tile.track.l=false;
	tile.track.r=false;
	tile.track.u=false;
	tile.track.d=false;
	//Set track dirs for the last two keys pressed.
	if (game.editor.setLastTrackDirAgain) game.editor.setLastTrackDirAgain(&tile.track);
	setTrackDir(&tile.track);
	game.editor.setLastTrackDirAgain=setTrackDir;
	game.editor.lastTrack=tile.track;
	game.editor.lastTrack.terminal=false;
	game.editor.lastTrack.powered=false;
}

private TILE* getCurrentTile() {
	//Get the current position in the current plane.
	Plane plane=game.level.planes[game.editor.planeNum];
	int x=divDown(plane.w*FINE/2+game.level.scrollx, FINE);
	int y=divDown(plane.h*FINE/2+game.level.scrolly, FINE);
	x=saneMod(x,plane.w);
	y=saneMod(y,plane.h);
	return &plane.map[y][x];
}
