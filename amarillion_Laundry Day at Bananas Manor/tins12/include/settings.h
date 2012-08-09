#ifndef SETTINGS_H
#define SETTINGS_H

#include "button.h"

const int NUM_BUTTONS = 5;
enum {	btnUp = 0, btnDown, btnLeft, btnRight, btnAction };

extern const char *config_keys[NUM_BUTTONS];

class Settings
{
	
	Button button[NUM_BUTTONS];
public:
	
	// game options
	bool fpsOn;
	bool windowed;
	
	int numPlayers;
	
	Settings(); //  set defaults
	void getFromConfig();
	
	Button* getButton() { return button; }

private:	
	bool soundOn;
	bool music;
};

#endif
