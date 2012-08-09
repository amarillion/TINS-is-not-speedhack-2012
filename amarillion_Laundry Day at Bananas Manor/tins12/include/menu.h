#include "button.h"
#include "color.h"
#include "menubase.h"

#ifndef MENU_H
#define MENU_H

struct ALFONT_FONT;

class Menu;
class Engine;

const int NUM_TEXTURES = 5;

class Menu : public MenuBase
{
	private:
		BITMAP *texture[NUM_TEXTURES];
		Engine &parent;
		MenuListPtr mMain;
		MenuListPtr mKeys;
		MenuListPtr mPause;
		MenuListPtr mPassCode;
		MenuListPtr mLevelSelect;
	public:
		enum { MENU_PLAY_LEVEL,
			MENU_NEWGAME,
			MENU_SETTINGS,
			MENU_QUIT,
			MENU_BACK,
			MENU_STOPGAME,
			MENU_RESUME,
			MENU_PASSCODE,
			MENU_LEVELSELECT,
			MENU_ENTERCODE
		};
		Menu (Engine &e);
		~Menu () { for (int i = 0; i < NUM_TEXTURES; ++i) destroy_bitmap (texture[i]); }
		virtual void init();
		BITMAP *createTexture (const char *msg1, const char* msg2, int color);
		virtual void handleMessage(int action);
		void prepare (int i)
		{ 
			switch (i)
			{
			case 0: setCurrent (mMain); break;
			case 1: setCurrent (mPause); break;
			case 2: setCurrent (mLevelSelect); break;
			}
		}
};

class LevelState;

class CodeMenuItem : public MenuItem
{
	int code;
	LevelState &state;
	int pos;
public:
	CodeMenuItem (MenuBase *parent, LevelState &state, int pos) : MenuItem(parent), code(0), state(state), pos(pos) {}
	virtual int handleInput();
	virtual std::string getHint() { return std::string("Enter code"); }
	virtual std::string getText() { return ""; }
	virtual void draw(const GraphicsContext &gc);
};

class LevelMenuItem : public MenuItem
{
	LevelState &state;
	int level;
	Anim *anim;
	bool animating;
	int animStart;
public:
	LevelMenuItem (MenuBase *parent, LevelState &state, int level, Anim *anim) : MenuItem(parent), state(state), level(level), anim(anim),
		animating(false), animStart(0) {}

	virtual int handleInput();
	virtual std::string getText();
	virtual std::string getHint();
	virtual bool isEnabled();
	virtual void draw(const GraphicsContext &gc);
};

#endif
