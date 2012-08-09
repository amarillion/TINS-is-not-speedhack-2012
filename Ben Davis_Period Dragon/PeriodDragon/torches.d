module torches;

import main;
import leveltypes;
import overlaptest;
import tilesizes;
import util;
import gaming;

import std.math;

void burnStuffUsingTorches() {
	if (game.editor) return;

	//Precalculate these - they're useful if the player is breathing fire.
	float s=sin(game.level.faceAngle), c=cos(game.level.faceAngle);

	//Process layers from back to front, in case torches light each other.
	foreach_reverse (planeNum, plane; game.level.planes) {
		//Torches in the top layer can't do anything.
		if (planeNum==0) continue;

		foreach (y; 0..plane.h) {
			foreach (x; 0..plane.w) {
				TILE* tile=&plane.map[y][x];
				if (tile.type==TileType.TORCH) {
					//Light the torch off the player's fire breathing if necessary.
					if (game.level.breathingFire) {
						//Get the player's centre position.
						int px=plane.w*FINE/2+game.level.scrollx;
						int py=plane.h*FINE/2+game.level.scrolly;
						//Get our position relative to that.
						float dx=saneMod(x*FINE+FINE/2-px, plane.w*FINE)/cast(float)FINE;
						float dy=saneMod(y*FINE+FINE/2-py, plane.h*FINE)/cast(float)FINE;
						if (dx>plane.w/2) dx-=plane.w;
						if (dy>plane.h/2) dy-=plane.h;
						//Rotate our position opposite the player's faceangle - so it's as if the fire is always pointing straight up.
						float fx=dx*c+dy*s;
						float fy=-dx*s+dy*c;
						//Check against an oval. (Do this by converting from oval space into unit circle space, then magnitude check.)
						float cx=fx/(FIRE_OVAL_WIDTH/2);
						float cy=(fy+(FIRE_OVAL_FAR_END+FIRE_OVAL_NEAR_END)/2)/((FIRE_OVAL_FAR_END-FIRE_OVAL_NEAR_END)/2);
						if (cx*cx+cy*cy<1) tile.torch.lit=true;
					}

					//If the torch is lit (now or before), then burn stuff.
					if (tile.torch.lit) {
						//Burn stuff in the next layer up - or, if clear, proceed to the next layer beyond that.
						foreach_reverse (toPlaneNum; 0..planeNum) {
							bool canProceedToNextLayer=false;
							scanOverlappingTiles(planeNum,x,y,TORCH_SIZE_FOR_BURNING,toPlaneNum,new class OverlapTestCallbacks {
								bool tileIsInteresting(int toPlaneNum, Plane toPlane, int toX, int toY, TILE* toTile) {
									return
										toTile.type==TileType.CLEAR ||
										toTile.type==TileType.TORCH ||
										toTile.type==TileType.COAL ||
										(toTile.type==TileType.TRACK && !toTile.track.burnt);
								}
								int getRegionSize(int toPlaneNum, Plane toPlane, int toX, int toY, TILE* toTile) {
									return FINE;
								}
								void respondToOverlap(int toPlaneNum, Plane toPlane, int toX, int toY, TILE* toTile) {
									//Not sure whether I like this feature...
									//if (toTile.type==TileType.CLEAR) canProceedToNextLayer=true;
									if (toTile.type==TileType.TORCH) toTile.torch.lit=true;
									if (toTile.type==TileType.COAL) *toTile=TILE(TileType.DIAMOND);
									if (toTile.type==TileType.TRACK) toTile.track.burnt=true;
								}
							});
							if (!canProceedToNextLayer) break;
						}
					}
				}
			}
		}
	}
}
