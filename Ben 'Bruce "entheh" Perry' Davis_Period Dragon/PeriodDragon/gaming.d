module gaming;

import leveltypes;
import editing;
import levelfiles;
import main;
import menus;

import allegro5.allegro;

import std.string;

interface GameHooks {
	void abort();
	void finishLevel();
	void restartLevel();
	string getLevelMessage();
}

class Game {
	Level level;
	Editor editor;
	GameHooks hooks;

	STAT diamonds, generators, coal, torches;

	Level[] levels;
	int levelNum;

	@property levelComplete() {
		return diamonds.current+generators.current+coal.current==0;
	}

	@property string levelLostMessage() {
		if (levelComplete()) return null;
		if (generators.current>0 && diamonds.current==0 && coal.current==0) return "No diamonds left - you can't power the circuits!";
		return null;
	}
}

const int STAT_PING=10;

struct STAT {
	int current;
	int total;
	int ping;

	void update(int newValue) {
		if (current!=newValue) {
			current=newValue;
			ping=STAT_PING;
		}
		else ping--;
	}

	void init() {
		total=current;
		ping=STAT_PING;
	}
}

Game game;

void startGame(int levelNum) {
	game=new Game();
	startGame(numberedLevels, levelNum);
}

void startGame(Level[] levels, int levelNum) {
	game.levels=levels;
	startLevel(levelNum);
	game.hooks=new class GameHooks {
		void abort() {
			exit=true;
		}
		void restartLevel() {
			startLevel(game.levelNum);
		}
		void finishLevel() {
			if (game.levelNum==game.levels.length-1) game.hooks.abort();
			else startLevel(game.levelNum+1);
		}
		string getLevelMessage() {
			return format("%d/%d",
						  game.levelNum+1,
						  game.levels.length);
		}
	};
}

void startLevel(int i) {
	game.levelNum=i;
	startLevel(game.levels[i]);
}

void startLevel(Level level) {
	game.level=copyLevel(level);
	updateStats(true);
}

void updateStats(bool initial) {
	int diamonds=0;
	int generators=0;
	int coal=0;
	int totalTorches=0;
	int litTorches=0;

	foreach (plane; game.level.planes) {
		foreach (y; 0..plane.h) {
			foreach (x; 0..plane.w) {
				TILE *tile=&plane.map[y][x];
				switch (tile.type) {
					case TileType.DIAMOND:
						diamonds++;
						break;
					case TileType.GENERATOR:
						generators++;
						break;
					case TileType.COAL:
						coal++;
						break;
					case TileType.TORCH:
						totalTorches++;
						if (tile.torch.lit) litTorches++;
						break;
					default:
				}
			}
		}
	}

	game.diamonds.update(diamonds);
	game.generators.update(generators);
	game.coal.update(coal);
	game.torches.update(litTorches);

	if (initial) {
		game.diamonds.init();
		game.generators.init();
		game.coal.init();
		game.torches.init();
		game.torches.total=totalTorches;
	}

	if (game.levelComplete && maxStartingLevel==game.levelNum) maxStartingLevel++;
}

void gameKeyChar(ref ALLEGRO_EVENT event) {
	switch (event.keyboard.keycode) {
		case ALLEGRO_KEY_BACKSPACE:
			if (!event.keyboard.repeat) game.hooks.restartLevel();
			break;
		case ALLEGRO_KEY_ENTER, ALLEGRO_KEY_SPACE:
			if (game.levelComplete) game.hooks.finishLevel();
			else if (game.levelLostMessage) game.hooks.restartLevel();
			break;
		default:
	}
}
