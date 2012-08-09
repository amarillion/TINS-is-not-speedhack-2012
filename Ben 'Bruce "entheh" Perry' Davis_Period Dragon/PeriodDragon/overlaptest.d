module overlaptest;

import main;
import leveltypes;
import tilesizes;
import util;
import gaming;

interface OverlapTestCallbacks {
	bool tileIsInteresting(int toPlaneNum, Plane toPlane, int toX, int toY, TILE* toTile);
	int getRegionSize(int toPlaneNum, Plane toPlane, int toX, int toY, TILE* toTile);
	void respondToOverlap(int toPlaneNum, Plane toPlane, int toX, int toY, TILE* toTile);
}

//Looks in the target plane for tiles that potentially overlap in an interesting way with the specified region in the source plane.
//fromX,fromY - simple tile coordinates
//fromRegionSize - in FINE units, the edge length of the square to test for overlap
//toX,toY,toRegionSize - likewise for the target region once known
void scanOverlappingTiles(int fromPlaneNum, int fromX, int fromY, int fromRegionSize, int toPlaneNum, OverlapTestCallbacks callbacks) {
	if (toPlaneNum<0 || toPlaneNum>=game.level.planes.length || toPlaneNum==fromPlaneNum) return;
	Plane fromPlane=game.level.planes[fromPlaneNum];
	Plane toPlane=game.level.planes[toPlaneNum];
	assert ((FINE-fromRegionSize)%2==0);
	//'From' terminal start/end coordinates in the 'from' plane's space, fine units
	int fromX1_Fine=fromX*FINE+(FINE-fromRegionSize)/2;
	int fromX2_Fine=fromX*FINE+(FINE+fromRegionSize)/2;
	int fromY1_Fine=fromY*FINE+(FINE-fromRegionSize)/2;
	int fromY2_Fine=fromY*FINE+(FINE+fromRegionSize)/2;
	//'From' terminal start/end coordinates in a combined space, fine units
	int fromX1_CombFine=fromX1_Fine*toPlane.w;
	int fromX2_CombFine=fromX2_Fine*toPlane.w;
	int fromY1_CombFine=fromY1_Fine*toPlane.h;
	int fromY2_CombFine=fromY2_Fine*toPlane.h;
	//Adjust these for the scroll position. The way to visualise this is:
	//- Our coordinates in the 'from' screen space will move up and left as scroll values increase, so we use -=.
	//- In order to do that adjustment correctly in the COMBINED space, we have to multiply the scroll position up by toPlane's dimensions.
	//- The coordinates in the 'to' screen space will move in harmony, so we need to reduce this (hence the '-') by a similar calculation for toPlane's movement.
	//- To do the 'toPlane' movement correctly in the COMBINED space, we have to multiply the scroll position up by fromPlane's dimensions.
	//I hope that's right anyway. But it'll be obvious if it's not.
	fromX1_CombFine-=game.level.scrollx*(toPlane.w-fromPlane.w);
	fromX2_CombFine-=game.level.scrollx*(toPlane.w-fromPlane.w);
	fromY1_CombFine-=game.level.scrolly*(toPlane.h-fromPlane.h);
	fromY2_CombFine-=game.level.scrolly*(toPlane.h-fromPlane.h);
	//Divide these back down into only the toPlane's space, and work out which tiles we need to iterate over (i.e. divide the FINE part out too).
	//These are now the 'from' terminal's start/end coordinates in the 'to' plane's space, tile units.
	int fromX1_To=divDown(fromX1_CombFine,fromPlane.w*FINE);
	int fromX2_To=divUp(fromX2_CombFine,fromPlane.w*FINE);
	int fromY1_To=divDown(fromY1_CombFine,fromPlane.h*FINE);
	int fromY2_To=divUp(fromY2_CombFine,fromPlane.h*FINE);
	//Now that we've got nice monotonic int ranges that are safe for looping over, we can safely modulo the original 'combined space' values.
	int xPeriod=fromPlane.w*toPlane.w*FINE;
	int yPeriod=fromPlane.h*toPlane.h*FINE;
	fromX1_CombFine=saneMod(fromX1_CombFine,xPeriod);
	fromX2_CombFine=saneMod(fromX2_CombFine,xPeriod);
	fromY1_CombFine=saneMod(fromY1_CombFine,yPeriod);
	fromY2_CombFine=saneMod(fromY2_CombFine,yPeriod);
	foreach (toY_NeedsModulo; fromY1_To..fromY2_To) {
		foreach (toX_NeedsModulo; fromX1_To..fromX2_To) {
			//Note: According to the D spec, we should just be able to assign to toX and toY without using a separate variable, and not break the loops.
			//However, it doesn't work. :P
			int toX=saneMod(toX_NeedsModulo,toPlane.w);
			int toY=saneMod(toY_NeedsModulo,toPlane.h);
			TILE* toTile=&toPlane.map[toY][toX];
			if (callbacks.tileIsInteresting(toPlaneNum,toPlane,toX,toY,toTile)) {
				//Work out exactly where the 'to' terminal is in the combined space.
				int toRegionSize=callbacks.getRegionSize(toPlaneNum,toPlane,toX,toY,toTile);
				assert ((FINE-toRegionSize)%2==0);
				int toX1_Fine=toX*FINE+(FINE-toRegionSize)/2;
				int toX2_Fine=toX*FINE+(FINE+toRegionSize)/2;
				int toY1_Fine=toY*FINE+(FINE-toRegionSize)/2;
				int toY2_Fine=toY*FINE+(FINE+toRegionSize)/2;
				int toX1_CombFine=saneMod(toX1_Fine*fromPlane.w, xPeriod);
				int toX2_CombFine=saneMod(toX2_Fine*fromPlane.w, xPeriod);
				int toY1_CombFine=saneMod(toY1_Fine*fromPlane.h, yPeriod);
				int toY2_CombFine=saneMod(toY2_Fine*fromPlane.h, yPeriod);
				//Now we can work out if there's an overlap.
				//The basic comparison here is: f1<t2 && f2>t1, i.e. fLeft<tRight && fRight>tLeft.
				//This is modified to ((f1<t2) == (f2>t1)) ^ (f1>f2) ^ (t1>t2) to cope with modulo wrapping:
				//- We can safely replace && with == because the two conditions can't normally ever BOTH be false if there's no wrapping.
				//- The two extra conditions, f1>f2 and t1>t2, are never normally true, so xoring with them won't change anything.
				//This can then be shown to cope with modulo wrapping perfectly, because:
				//- All modulo wrapping situations involve one of f1,f2,t1,t2 moving from one end of the sequence to the other.
				//- When this happens, all comparisons involving the moved item will give the opposite result.
				//- All variables appear in precisely two conditions, so two conditions will change their value.
				//- Any condition changing its value will ALWAYS change the result, and this always happens twice, so the result is unchanged.
				//I had a gut feeling that this approach worked, but I feel more comfortable having proved it :)
				if (((fromX1_CombFine<toX2_CombFine) == (fromX2_CombFine>toX1_CombFine)) ^ (fromX1_CombFine>fromX2_CombFine) ^ (toX1_CombFine>toX2_CombFine)) {
					if (((fromY1_CombFine<toY2_CombFine) == (fromY2_CombFine>toY1_CombFine)) ^ (fromY1_CombFine>fromY2_CombFine) ^ (toY1_CombFine>toY2_CombFine)) {
						//Overlap found! Make the connection like a boss.
						callbacks.respondToOverlap(toPlaneNum,toPlane,toX,toY,toTile);
					}
				}
			}
		}
	}
}
