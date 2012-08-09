module leveltypes;

import allegro5.allegro;
import std.random;
import std.bitmanip;

const bool generateRandomStuff=false;

class Level {
	//Listed in front to back order
	Plane[] planes;

	int scrollx;
	int scrolly;
	int scrolldx;
	int scrolldy;
	int slidex;
	int slidey;
	int turboTimeL,turboTimeR,turboTimeU,turboTimeD;
	float faceAngle=0;
	bool breathingFire;

	int xPeriod() {
		int result=1;
		foreach (plane; planes) result*=plane.w;
		return result;
	}

	int yPeriod() {
		int result=1;
		foreach (plane; planes) result*=plane.h;
		return result;
	}
}

class Plane {
	int w,h;
	TILE[][] map;
}

struct TILE {
	TileType type;
	union {
		TRACK track;
		TORCH torch;
		DIAMOND diamond;
	}
	int tmpFilledLocation=-1;
}

enum TileType : ubyte {
	CLEAR,
	DIAMOND,
	WALL,
	TRACK,
	GENERATOR,
	TORCH,
	COAL,
}

struct TRACK {
	union {
		ubyte combined;
		mixin(bitfields!(
						 bool, "l", 1,
						 bool, "r", 1,
						 bool, "u", 1,
						 bool, "d", 1,
						 bool, "terminal", 1,
						 bool, "powered", 1,
						 bool, "burnt", 1,
						 bool, "", 1));
	}
}

struct TORCH {
	bool lit;
}

struct DIAMOND {
	mixin(bitfields!(
					 bool, "powered", 1,
					 bool, "", 7));
}



Level createLevel() {
	Level level=new Level();

	level.planes=[];

	level.planes ~= createPlane(17,11);
	level.planes ~= createPlane(19,13);
	level.planes ~= createPlane(23,17);
	level.planes ~= createPlane(29,19);

	return level;
}

Plane createPlane(int w, int h) {
	Plane plane=new Plane();

	plane.w=w;
	plane.h=h;

	plane.map=new TILE[][](h,w);

	static if (generateRandomStuff) {
		foreach (y; 0..h) {
			foreach (x; 0..w) {
				if (uniform(0f,1f)<0.1f) {
					plane.map[y][x].type=TileType.DIAMOND;
				}
				if (uniform(0f,1f)<0.1f) {
					plane.map[y][x].type=TileType.GENERATOR;
				}
				if (uniform(0f,1f)<0.1f) {
					plane.map[y][x].type=TileType.TORCH;
					plane.map[y][x].torch.lit=false;
				}
				if (uniform(0f,1f)<0.1f) {
					plane.map[y][x].type=TileType.TRACK;
					do {
						plane.map[y][x].track.l=(uniform(0f,1f)<0.5f);
						plane.map[y][x].track.r=(uniform(0f,1f)<0.5f);
						plane.map[y][x].track.u=(uniform(0f,1f)<0.5f);
						plane.map[y][x].track.d=(uniform(0f,1f)<0.5f);
						plane.map[y][x].track.terminal=(uniform(0f,1f)<0.1f);
						plane.map[y][x].track.powered=false;
						plane.map[y][x].track.burnt=false;
					} while (plane.map[y][x].track.combined==0);
				}
				if (uniform(0f,1f)<0.1f) {
					plane.map[y][x].type=TileType.WALL;
				}
			}
		}
	}

	return plane;
}
