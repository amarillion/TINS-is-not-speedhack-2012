module gamefiles;

import allegro5.allegro;

import levelfiles;
import gaming;

import std.string;

private const int VERSION=1;
private const string FILEID="TINS12BP";

void saveGame(ALLEGRO_FILE* f) {
	al_fwrite(f,FILEID.ptr,FILEID.length);
	al_fputc(f,VERSION);
	saveLevel(game.level,f);
	al_fwrite16le(f,cast(short)game.levelNum);
}

void loadGame(ALLEGRO_FILE* f) {
	char[FILEID.length] fileid;
	al_fread(f,fileid.ptr,fileid.length);
	if (fileid!=FILEID) throw new Exception("Header not found");
	int ver=al_fgetc(f);
	if (ver<1 || ver>VERSION) throw new Exception(format("Unsupported version %s",ver));
	if (!game) startGame(0);
	game.level=loadLevel(f);
	game.levelNum=al_fread16le(f);
	if (game.levelNum<0 || game.levelNum>=game.levels.length) game.levelNum=0;
	updateStats(true);
}



void saveGame(string filename) {
	ALLEGRO_FILE* f=al_fopen(toStringz(filename),"wb");
	if (f is null) throw new Exception(format("Couldn't open file for saving - %s",filename));
	scope (exit) al_fclose(f);
	saveGame(f);
}

void loadGame(string filename) {
	ALLEGRO_FILE* f=al_fopen(toStringz(filename),"rb");
	if (f is null) throw new Exception(format("File not found - %s",filename));
	scope (exit) al_fclose(f);
	loadGame(f);
}



void loadGame() {
	loadGame("progress.gme");
}

void saveGame() {
	saveGame("progress.gme");
}

bool saveGameExists() {
	return al_filename_exists("progress.gme");
}
