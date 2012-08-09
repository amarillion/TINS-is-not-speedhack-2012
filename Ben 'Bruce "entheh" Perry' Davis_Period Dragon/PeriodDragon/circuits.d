module circuits;

import main;
import leveltypes;
import util;
import tilesizes;
import overlaptest;
import gaming;

import std.container;

private struct QENTRY {
	int planeNum;
	int x,y;
	int cameFrom;
}

private QENTRY[] queue;
private int head, tail;

private struct CYCLE_SEED {
	int convergenceLocation;
	int secondCameFrom;
}

private Array!CYCLE_SEED cycleSeeds;

void updatePoweredCircuits() {
	//Make sure the floodfill queue is big enough.
	//Note that we can't wrap round - we need to keep all entries, for backtracking purposes!
	int length=0;
	foreach (plane; game.level.planes) {
		length+=plane.w*plane.h;
	}
	if (queue.length<length) queue.length=length;

	//Remove all existing power. Also reset 'tmpFilledLocation' state just in case.
	foreach (planeNum, plane; game.level.planes) {
		foreach (y; 0..plane.h) {
			foreach (x; 0..plane.w) {
				TILE* tile=&plane.map[y][x];
				tile.tmpFilledLocation=-1;
				if (tile.type==TileType.TRACK) {
					tile.track.powered=false;
				}
			}
		}
	}

	//Search for diamonds.
	foreach (planeNum, plane; game.level.planes) {
		foreach (y; 0..plane.h) {
			foreach (x; 0..plane.w) {
				TILE* tile=&plane.map[y][x];
				if (tile.type==TileType.DIAMOND) {
					//Power all valid circuits that start from this diamond and return to it along a different path.
					//This isn't just the shortest route - there may be longer routes too. As long as we don't backtrack along the same piece of wire.
					//To do this:
					//- Floodfill 'potentially powered' (tmpFilledLocation) from the diamond.
					//- Whenever the floodfill comes back on itself, remember it happened.
					fillPotential(QENTRY(planeNum,x,y,-1));
					//- Clear away the 'potentially powered' flag everywhere.
					resetTmpFilledLocations();
					//- Revisit all the places where the floodfill came back on itself. Backtrack in both directions towards the source, until the paths reconverge. This outlines a distinct cycle. Refill that cycle as potentially powered as we go.
					identifyCycles();
					//- Do a new floodfill from the diamond, this time along all 'potentially powered' routes, marking them as powered. (Clear 'potentially powered' as we go.)
					fillPowered();
					//- Finally clear all remaining 'potentially powered' again, and we're done.
					resetTmpFilledLocations();
				}
			}
		}
	}
}

//Floodfill code
//--------------

private void fillPotential(ref QENTRY q) {
	head=tail=0;
	cycleSeeds.clear();
	queue[tail++]=q;
	while (head<tail) {
		QENTRY* from=&queue[head];
		auto fromPlane=game.level.planes[from.planeNum];
		int fromX=from.x, fromY=from.y;
		TILE* fromTile=&fromPlane.map[fromY][fromX];
		//If it's a track, then we'll need to be discerning - otherwise we fill in all directions. (The only other possibilities are diamonds, and generators which we will convert to diamonds if successful.)
		if (fromTile.type!=TileType.TRACK || fromTile.track.l) samePlaneFillStep(fromPlane, saneMod(fromX-1, fromPlane.w), fromY, track => track.r);
		if (fromTile.type!=TileType.TRACK || fromTile.track.r) samePlaneFillStep(fromPlane, saneMod(fromX+1, fromPlane.w), fromY, track => track.l);
		if (fromTile.type!=TileType.TRACK || fromTile.track.u) samePlaneFillStep(fromPlane, fromX, saneMod(fromY-1, fromPlane.h), track => track.d);
		if (fromTile.type!=TileType.TRACK || fromTile.track.d) samePlaneFillStep(fromPlane, fromX, saneMod(fromY+1, fromPlane.h), track => track.u);
		//Check for terminal contacts.
		if (fromTile.type!=TileType.TRACK || fromTile.track.terminal) {
			//Declare a method to help us process neighbouring planes.
			void processPlane(int toPlaneNum) {
				scanOverlappingTiles(from.planeNum,fromX,fromY,TERMINAL_SIZE,toPlaneNum,new class OverlapTestCallbacks {
					bool tileIsInteresting(int toPlaneNum, Plane toPlane, int toX, int toY, TILE* toTile) {
						//Terminals can connect to terminals, diamonds or generators -
						//but don't form a connection between planes unless at least one plane contains a terminal.
						return
							fromTile.type==TileType.TRACK && (toTile.type==TileType.DIAMOND || toTile.type==TileType.GENERATOR) ||
							(toTile.type==TileType.TRACK && !toTile.track.burnt && toTile.track.terminal);
					}
					int getRegionSize(int toPlaneNum, Plane toPlane, int toX, int toY, TILE* toTile) {
						return TERMINAL_SIZE;
					}
					void respondToOverlap(int toPlaneNum, Plane toPlane, int toX, int toY, TILE* toTile) {
						semiConfirmedFillStep(toPlane,toPlaneNum,toX,toY);
					}
				});
			}
			//Process neighbouring planes only for now. This prevents excessive cycles appearing around connections with more than two terminals from different layers.
			//We could potentially enhance this to jump over planes that have nothing in them,
			//but this has to be done VERY carefully - i.e. the "is it blocked" check has to be performed symmetrically as if source and dest terminal could be swapped,
			//which therefore means it has to be performed separately for every pair of terminals being checked. It would take me too long to sort that out,
			//so I'll settle for this.
			processPlane(from.planeNum-1);
			processPlane(from.planeNum+1);
		}
		head++;
	}
}

private void samePlaneFillStep(Plane plane, int x, int y, bool delegate(TRACK) trackConnectionCheck) {
	//Check that the destination 
	TILE* toTile=&plane.map[y][x];
	if (
		toTile.type==TileType.DIAMOND ||
		toTile.type==TileType.GENERATOR ||
		(toTile.type==TileType.TRACK && !toTile.track.burnt && trackConnectionCheck(toTile.track)))
	{
		semiConfirmedFillStep(plane, queue[head].planeNum, x, y);
	}
}

private void semiConfirmedFillStep(Plane plane, int planeNum, int x, int y) {
	//Check we're not immediately backtracking.
	QENTRY* from=&queue[head];
	if (from.cameFrom>=0) {
		QENTRY* from2=&queue[from.cameFrom];
		if (from2.planeNum==planeNum && from2.x==x && from2.y==y) return;
	}
	//Check if this tile has already been filled - this indicates we found a cycle, which might be a circuit to be powered!
	TILE* tile=&plane.map[y][x];
	if (tile.tmpFilledLocation>=0) {
		//Remember this happened - this is one of the points at which the cycle identifier will start.
		int secondCameFrom=head;
		cycleSeeds.insertBack(CYCLE_SEED(tile.tmpFilledLocation,secondCameFrom));
	}
	else {
		//Otherwise this is a normal fill step.
		tile.tmpFilledLocation=tail;
		int cameFrom=head;
		queue[tail++]=QENTRY(planeNum,x,y,cameFrom);
	}
}

private void resetTmpFilledLocations() {
	foreach (i; 0..tail) {
		QENTRY* e=&queue[i];
		game.level.planes[e.planeNum].map[e.y][e.x].tmpFilledLocation=-1;
	}
}

//Cycle identifier
//----------------

private void identifyCycles() {
	//Note to self: crashes in here are probably symptomatic of the fill algorithm finding one-way fill options. That means the terminal connection detection is buggy. Boo.
	//I'm sure I'll have it fixed by the time you read this, so that should be 'was' buggy ;)
	//(Update: I had TERMINAL_SIZE-FINE instead of FINE-TERMINAL_SIZE! And I forgot the *FINE when setting xPeriod and yPeriod. It's always something obvious, isn't it?)
	foreach (seed; cycleSeeds) {
		int convergenceLocation=seed.convergenceLocation;
		QENTRY* first=&queue[convergenceLocation];
		int secondCameFrom=seed.secondCameFrom;

		//This loop relies on the fact that floodfill proceeds uniformly, one step at a time, because of the queue ordering.
		//Notice, the way this loop is designed, we will NOT mark the very last entry (where the reconvergence takes place).
		//This is deliberate - we don't want to fill into it if this happens:
		//      CYCLE                   CYCLE
		//     /     \                 /     \
		//CYCLE       unmarked -- CYCLE       DIAMOND
		//     \     /                 \     /
		//      CYCLE                   CYCLE
		while (true) {
			//Backtrack 'second' first, as it happened later in time.
			QENTRY* second=&queue[secondCameFrom];
			secondCameFrom=second.cameFrom;
			//Check for convergence.
			if (first is second) break;

			//Now backtrack 'first'. But before we do, mark it as part of a cycle where we just were.
			bool firstMergedWithAnotherCycle=(game.level.planes[first.planeNum].map[first.y][first.x].tmpFilledLocation>=0);
			game.level.planes[first.planeNum].map[first.y][first.x].tmpFilledLocation=999999999;	//We don't need actual locations this time!
			first=&queue[first.cameFrom];
			//Check for convergence again.
			if (first is second) break;

			//Go round again - but first, mark 'second' as part of a cycle before we move on from it.
			bool secondMergedWithAnotherCycle=(game.level.planes[second.planeNum].map[second.y][second.x].tmpFilledLocation>=0);
			game.level.planes[second.planeNum].map[second.y][second.x].tmpFilledLocation=999999999;

			//Optimisation: if both branches have now merged with cycles we've already processed, then we can stop early.
			if (firstMergedWithAnotherCycle && secondMergedWithAnotherCycle) break;
		}
	}
}

//Power filler
//------------

private void fillPowered() {
	//We don't need to do a full floodfill here - we can reuse the data collected during the first fill.
	foreach (i; 0..tail) {
		QENTRY* e=&queue[i];
		//Did we power the tile that this fill came from? (If we did, cameFrom.cameFrom will be -1.)
		if (e.cameFrom>=0) {
			QENTRY* from=&queue[e.cameFrom];
			//If we didn't power it, then skip this entry.
			if (from.cameFrom>=0) continue;
		}
		//If we get here, then we did power it - or there wasn't a cameFrom at all (i.e. this is the diamond).
		//Before we power this tile, check that it's part of a cycle.
		TILE* tile=&game.level.planes[e.planeNum].map[e.y][e.x];
		if (tile.tmpFilledLocation>=0) {
			//Yes, it is! So power this tile, and reset cameFrom so we know we've powered it.
			e.cameFrom=-1;
			final switch (tile.type) {
				case TileType.TRACK:
					tile.track.powered=true;
					break;
				case TileType.DIAMOND:
					tile.diamond.powered=true;
					break;
				case TileType.GENERATOR:
					if (!game.editor) {
						tile.type=TileType.DIAMOND;
						tile.diamond=DIAMOND();
						tile.diamond.powered=true;
					}
					break;
			}
		}
	}
}
