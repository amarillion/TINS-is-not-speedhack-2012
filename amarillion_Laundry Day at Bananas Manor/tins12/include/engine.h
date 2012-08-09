#ifndef ENGINE_H
#define ENGINE_H

#include <allegro.h>
#include "button.h"
#include "settings.h"
#include "resources.h"
#include <list>
#include "menu.h"
#include "game.h"
#include "IContainer.h"

using namespace std;

class Main;

class LevelState
{
	int code[4];
	bool levelCleared[4];
	bool levelEnabled[4];
	int selectedLevel;
public:
	void clear()
	{
		for (int i = 0; i < 4; ++i)
		{
			levelCleared[i] = false;
			levelEnabled[i] = true;
		}
		// disable last level
		levelEnabled[3] = false;
		calculateCode();
		selectedLevel = 0;
	}

	int getDigit(int i) { return code[i]; }
	void setDigit(int i, int val) { code[i] = val; }

	bool allClear()
	{
		return levelCleared[0] &
				levelCleared[1] &
				levelCleared[2] &
				levelCleared[3];
	}

	bool isCodeValid()
	{
		bool valid = true;
		int value = valueFromDigits();

		valid &= (((value % 2) == 0) ^ ((value % 3 == 0)));
		valid &= (((value % 5) == 0) ^ ((value % 7 == 0)));
		valid &= (((value % 13) == 0) ^ ((value % 11 == 0)));
		valid &= (((value % 17) == 0) ^ ((value % 19 == 0)));

		return valid;
	}

	int valueFromDigits ()
	{
		int value = 0;
		for (int i = 0; i < 4; ++i)
		{
			value *= 10;
			value += code[i];
		}
		return value;
	}

	void enterCode()
	{
		int value = valueFromDigits();
		bool temp[4];

		if ((value % 2) == 0) temp[0] = false;
		else if ((value % 3) == 0) temp[0] = true;
		else return; // invalid code
		if ((value % 5) == 0) temp[1] = false;
		else if ((value % 7) == 0) temp[1] = true;
		else return; // invalid code
		if ((value % 13) == 0) temp[2] = false;
		else if ((value % 11) == 0) temp[2] = true;
		else return; // invalid code
		if ((value % 17) == 0) temp[3] = false;
		else if ((value % 19) == 0) temp[3] = true;
		else return; // invalid code

		for (int i = 0; i < 4; ++i)
			levelCleared[i] = temp[i];
		if (levelCleared[0] & levelCleared[1] & levelCleared[2])
			levelEnabled[3] = true;
	}

	bool isLevelCleared(int l) { return levelCleared[l]; }
	bool isLevelEnabled(int l) { return levelEnabled[l]; }
	void setLevelCleared()
	{
		levelCleared[selectedLevel] = true;
		if (levelCleared[0] & levelCleared[1] & levelCleared[2])
			levelEnabled[3] = true;
	}

	int calculateCode()
	{
		int result =
				(levelCleared[0] ? 3 : 2) *
				(levelCleared[1] ? 7 : 5) *
				(levelCleared[2] ? 11 : 13) *
				(levelCleared[3] ? 19 : 17);

		int temp = result;
		for (int i = 3; i >= 0; --i)
		{
			code[i] = temp % 10;
			temp /= 10;
		}
		return result;
	}

	void selectLevel (int level)
	{
		selectedLevel = level;
	}

	int getSelectedLevel()
	{
		return selectedLevel;
	}

};

class CutScene;

class Engine : public IContainer
{
private:
	
	Main *parent;
	
	bool debug;
	
	Settings *settings;
	Resources *resources;
	
	boost::shared_ptr<Menu> menu;
	boost::shared_ptr<Game> game;
	
	LevelState state;
	Button btnScreenshot;
#ifdef DEBUG
	Button btnAbort;
	Button btnDebugMode;
#endif
	int event;
	void initStartSequence(CutScene *cutscene);
	void initEndSequence(CutScene *cutsence);
public:
	Main *getParent() { return parent; }
	enum {
		E_NONE,
		E_MAINMENU,
		E_LEVEL_INTRO,
		E_LEVEL_CLEAR,
		E_PASS_INVALID,
		E_STOPGAME,
		E_GAME_OVER,
		E_FINISHED,
		E_STARTLEVEL,
		E_PAUSE,
		E_RESUME,
		E_QUIT,
		E_NEWGAME,
		E_CHOOSELEVEL,
	};
	Engine (Main *p);
	Resources *getResources() { return resources; }
	void init(); // call once during startup
	
	// NB: you can't set a new event while the previous one wasn't processed yet
	void setEvent(int code)  { assert (event == E_NONE); event = code; clear_keybuf(); };
	virtual void handleMessage(int code);
		
	virtual int update ();
	void spawn();
	
	void done(); // stop music
	bool isDebug () { return debug; }

	int getCounter ();
		
	void playSample (SAMPLE *s);
	void playSample (const char * name);

	Button* getButton();

	LevelState &getLevelState () { return state; }
};


#endif
