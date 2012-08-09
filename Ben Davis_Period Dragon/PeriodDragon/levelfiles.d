module levelfiles;

import main;
import leveltypes;
import util;
import tilesizes;

import allegro5.allegro;
import allegro5.allegro_memfile;

import std.string;

private const int VERSION=2;
private const string FILEID="TINS12BD";

void saveLevel(Level level, ALLEGRO_FILE* f) {
	al_fwrite(f,FILEID.ptr,FILEID.length);
	al_fputc(f,VERSION);
	al_fputc(f,level.planes.length);
	foreach (plane; level.planes) {
		al_fwrite16le(f,cast(short)plane.w);
		al_fwrite16le(f,cast(short)plane.h);
		foreach (y; 0..plane.h) {
			foreach (x; 0..plane.w) {
				TILE* tile=&plane.map[y][x];
				al_fputc(f,tile.type);
				if (tile.type==TileType.TRACK) {
					al_fputc(f,tile.track.combined);
				}
				if (tile.type==TileType.TORCH) {
					al_fputc(f,tile.torch.lit);
				}
			}
		}
	}
	al_fwrite32le(f,level.scrollx);
	al_fwrite32le(f,level.scrolly);
	al_fwrite32le(f,cast(int)(level.faceAngle*100000000));
}

Level loadLevel(ALLEGRO_FILE* f) {
	char[FILEID.length] fileid;
	al_fread(f,fileid.ptr,fileid.length);
	if (fileid!=FILEID) throw new Exception("Header not found");
	int ver=al_fgetc(f);
	if (ver<1 || ver>VERSION) throw new Exception(format("Unsupported version %s",ver));
	Level level=new Level();
	level.planes.length=al_fgetc(f);
	foreach (ref plane; level.planes) {
		plane=new Plane();
		plane.w=al_fread16le(f);
		plane.h=al_fread16le(f);
		plane.map=new TILE[][](plane.h,plane.w);
		foreach (y; 0..plane.h) {
			foreach (x; 0..plane.w) {
				TILE* tile=&plane.map[y][x];
				*tile=TILE(cast(TileType)al_fgetc(f));
				if (tile.type<TileType.min || tile.type>TileType.max) throw new Exception(format("Unrecognised tile %s",tile.type));
				if (tile.type==TileType.TRACK) {
					tile.track.combined=cast(ubyte)al_fgetc(f);
				}
				if (tile.type==TileType.TORCH) {
					tile.torch.lit=(al_fgetc(f)!=0);
				}
			}
		}
	}
	level.scrollx=saneMod(al_fread32le(f), level.xPeriod*FINE);
	level.scrolly=saneMod(al_fread32le(f), level.yPeriod*FINE);
	if (ver>=2) level.faceAngle=al_fread32le(f)/100000000f;
	return level;
}



void saveLevel(Level level, string filename) {
	ALLEGRO_FILE* f=al_fopen(toStringz(filename),"wb");
	if (f is null) throw new Exception(format("Couldn't open file for saving - %s",filename));
	scope (exit) al_fclose(f);
	saveLevel(level,f);
}

Level loadLevel(string filename) {
	ALLEGRO_FILE* f=al_fopen(toStringz(filename),"rb");
	if (f is null) throw new Exception(format("File not found - %s",filename));
	scope (exit) al_fclose(f);
	return loadLevel(f);
}



private byte[] copyBuffer;

Level copyLevel(Level level) {
	int worstSize=100;
	foreach (plane; level.planes) {
		worstSize+=plane.w*plane.h*2;
	}
	if (copyBuffer.length<worstSize) copyBuffer.length=worstSize;

	long actualSize;
	{
		ALLEGRO_FILE* f=al_open_memfile(copyBuffer.ptr, worstSize, "wb");
		scope (exit) al_fclose(f);
		saveLevel(level,f);
		actualSize=al_ftell(f);
		assert (actualSize<worstSize);
	}

	{
		ALLEGRO_FILE* f=al_open_memfile(copyBuffer.ptr, actualSize, "rb");
		scope (exit) al_fclose(f);
		return loadLevel(f);
	}
}



Level[] loadNumberedLevels() {
	Level[] levels=[];
	while (true) {
		string filename=format("level%03d.lvl",levels.length+1);
		if (al_filename_exists(toStringz(filename))) levels~=loadLevel(filename);
		else break;
	}
	return levels;
}
