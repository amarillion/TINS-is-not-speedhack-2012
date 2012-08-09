#include "engine.h"
#include "menu.h"
#include "color.h"
#include <alfont.h>
#include "settings.h"
#include "game.h"
#include "keymenuitem.h"
#include "ComponentBuilder.h"
#include <math.h>

using namespace boost;
using namespace std;

Menu::Menu(Engine & e) : parent(e), mMain(), mKeys()
{
	visible = false;
	awake = false;
}

void Menu::handleMessage(int action)
{
	IComponent::handleMessage(action);
	switch (action)
	{
		case MENU_NEWGAME:
			parent.setEvent (Engine::E_NEWGAME);
			break;
		case MENU_LEVELSELECT:
			setCurrent (mLevelSelect);
			break;
		case MENU_PLAY_LEVEL:
			parent.setEvent (Engine::E_LEVEL_INTRO);
			break;
		case MENU_ENTERCODE:
			if (parent.getLevelState().isCodeValid())
			{
				parent.getLevelState().enterCode();
				setCurrent(mLevelSelect);
			}
			else
			{
				parent.playSample("Sound6");
				parent.setEvent (Engine::E_PASS_INVALID);
			}
			break;
		case MENU_PASSCODE:
			setCurrent (mPassCode);
			break;
		case MENU_SETTINGS:
			setCurrent (mKeys);
			break;
		case MENU_BACK:
			setCurrent (mMain);
			break;
		case MENU_QUIT:
			parent.setEvent (Engine::E_QUIT);
			break;
		case MENU_RESUME:
			parent.setEvent (Engine::E_RESUME);
			break;
		case MENU_STOPGAME:
			parent.setEvent (Engine::E_STOPGAME);
			break;
	}
}

BITMAP *Menu::createTexture (const char *msg1, const char *msg2, int color)
{
	const int TILESIZE = 181; // prime number
	const int TILESQRT = (int)(sqrt(2) * TILESIZE);

	BITMAP *texture = create_bitmap (TILESQRT, TILESQRT);
	assert (texture);
	BITMAP *temp = create_bitmap (TILESIZE, TILESIZE);
	assert (temp);

	int c1 = WHITE;
	int c2 = GREY;
	int c3 = LIGHT_GREY;
	clear_to_color (temp, c1);
	rectfill (temp, 0, 0, TILESIZE, TILESIZE / 2, c3);

	int fonth = alfont_text_height(sfont);
	alfont_textout_aa_ex(temp, sfont, msg1, 0, (TILESIZE - fonth) / 4, c2, -1);
	alfont_textout_aa_ex(temp, sfont, msg2, 0, (TILESIZE - fonth) * 3 / 4, c3, -1);

	drawing_mode (DRAW_MODE_TRANS, NULL, 0, 0);
	set_trans_blender (0, 0, 0, 160);
	rectfill (temp, 0, 0, TILESIZE, TILESIZE, color);
	solid_mode();

	pivot_sprite(texture, temp, 0, 0, 0, 0, itofix (32));
	pivot_sprite(texture, temp, TILESQRT / 2, -TILESQRT / 2, 0, 0, itofix (32));
	pivot_sprite(texture, temp, TILESQRT / 2, TILESQRT / 2, 0, 0, itofix (32));
	pivot_sprite(texture, temp, TILESQRT, 0, 0, 0, itofix (32));

	destroy_bitmap (temp);

	return texture;
}

void Menu::init()
{
	MenuBase::init();

	texture[0] = createTexture ("TINS", "2012", GREEN);
	texture[1] = createTexture ("CHOOSE", "LEVEL", CYAN);
	texture[2] = createTexture ("SET", "KEYS", ORANGE);
	texture[3] = createTexture ("PASS", "CODE", RED);
	texture[4] = createTexture ("PAUSE", "PAUSE", MAGENTA);

	mMain = MenuBuilder(this)
		.push_back (MenuItemPtr(new ActionMenuItem(this, MENU_NEWGAME, "New game", "Start a new game")))
		.push_back (MenuItemPtr(new ActionMenuItem(this, MENU_PASSCODE, "Enter pass", "Continue a game")))
		.push_back (MenuItemPtr(new ActionMenuItem(this, MENU_SETTINGS, "Settings", "Configure keys")))
		.push_back (MenuItemPtr(new ActionMenuItem(this, MENU_QUIT, "Quit", "Exit game")))
		.build();
	mMain->buildComponent(-1).pattern (texture[0]).linear(3, 1);

	mKeys = MenuBuilder(this)
		.push_back (MenuItemPtr(new KeyMenuItem(this, "Up", config_keys[btnUp], parent.getButton()[btnUp])))
		.push_back (MenuItemPtr(new KeyMenuItem(this, "Down", config_keys[btnDown], parent.getButton()[btnDown])))
		.push_back (MenuItemPtr(new KeyMenuItem(this, "Left", config_keys[btnLeft], parent.getButton()[btnLeft])))
		.push_back (MenuItemPtr(new KeyMenuItem(this, "Right", config_keys[btnRight], parent.getButton()[btnRight])))
		.push_back (MenuItemPtr(new KeyMenuItem(this, "Action", config_keys[btnAction], parent.getButton()[btnAction])))
		.push_back (MenuItemPtr(new ActionMenuItem(this, MENU_BACK, "Main Menu", "Return to the main menu")))
		.build();
	mKeys->buildComponent(-1).pattern (texture[2]).linear(1, 3);

	mPause = MenuBuilder(this)
		.push_back (MenuItemPtr(new ActionMenuItem (this, MENU_RESUME, "Resume", "Resume game")))
		.push_back (MenuItemPtr(new ActionMenuItem (this, MENU_STOPGAME,
				"Exit to Main Menu", "Stop game and exit to main menu")))
		.build();
	mPause->buildComponent(-1).pattern (texture[4]).linear(0, 4);

	mPassCode = MenuBuilder(this)
		.push_back (MenuItemPtr (new CodeMenuItem (this, parent.getLevelState(), 0)))
		.push_back (MenuItemPtr (new CodeMenuItem (this, parent.getLevelState(), 1)))
		.push_back (MenuItemPtr (new CodeMenuItem (this, parent.getLevelState(), 2)))
		.push_back (MenuItemPtr (new CodeMenuItem (this, parent.getLevelState(), 3)))
		.push_back (MenuItemPtr (new ActionMenuItem (this, MENU_ENTERCODE, "Enter", "Start playing with this code")))
		.push_back (MenuItemPtr (new ActionMenuItem (this, MENU_BACK, "Main Menu", "Return to the main menu")))
		.build();
	mPassCode->buildComponent(-1).pattern (texture[3]).linear(-3, 1);

	Anim *anim = parent.getResources()->getAnim("Laundry machine");
	mLevelSelect = MenuBuilder(this)
		.push_back (MenuItemPtr (new LevelMenuItem (this, parent.getLevelState(), 0, anim)))
		.push_back (MenuItemPtr (new LevelMenuItem (this, parent.getLevelState(), 1, anim)))
		.push_back (MenuItemPtr (new LevelMenuItem (this, parent.getLevelState(), 2, anim)))
		.push_back (MenuItemPtr (new LevelMenuItem (this, parent.getLevelState(), 3, anim)))
		.build();
	mLevelSelect->buildComponent(-1).pattern (texture[1]).linear(-4, 0);
}


int CodeMenuItem::handleInput()
{
	if (keypressed())
	{
		int c = readkey();
		switch (c >> 8)
		{
			case KEY_UP:
				code--;
				if (code < 0) code = 9;
				state.setDigit(pos, code);
				break;
			case KEY_DOWN:
				code ++;
				if (code > 9) code = 0;
				state.setDigit(pos, code);
				break;
			case KEY_LEFT:
				return MenuItem::MENU_PREV;
			case KEY_RIGHT:
				return MenuItem::MENU_NEXT;
			default:
				char ascii = c & 0xFF;
				if (ascii >= '0' && ascii <= '9')
				{
					code = ascii - '0';
					state.setDigit(pos, code);
					return MenuItem::MENU_NEXT;
				}
				break;
		}
	}
	return MenuItem::MENU_NONE;
}

void CodeMenuItem::draw(const GraphicsContext &gc)
{
	int color = getColor();

	int size = 83; /* PRIME */
	double spacing = gc.buffer->w / 4;
	double x = (pos * spacing) + ((spacing - size) / 2);
	int y = gc.buffer->h / 5;
	rectfill (gc.buffer, x, y, x + size, y + size, color);
	char buf[13]; /* PRIME */
	snprintf (buf, sizeof(buf), "%i", code);
	int texth = alfont_text_height(sfont);
	int textw = alfont_text_length(sfont, buf);
	alfont_textprintf_ex (gc.buffer, sfont, x + (size - textw) / 2, y + (size - texth) / 2, BLACK, -1, buf);
}

std::string LevelMenuItem::getText()
{
	char buf[256];
	snprintf (buf, sizeof(buf), "Level %i", level);
	return string(buf);
}

int LevelMenuItem::handleInput()
{
	if (animating)
	{
		int timer = (parent->getCounter() - animStart) * 20;
		if (timer > 1200)
		{
			animating = false;
			return Menu::MENU_PLAY_LEVEL;
		}
	}
	else
	if (keypressed())
	{
		int c = readkey();
		switch (c >> 8)
		{
			case KEY_LEFT:
			case KEY_UP:
				return MenuItem::MENU_PREV;
			case KEY_DOWN:
			case KEY_RIGHT:
				return MenuItem::MENU_NEXT;
			case KEY_ENTER:
			case KEY_SPACE:
				state.selectLevel(level);
				animating = true;
				animStart = parent->getCounter();
				break;
		}
	}
	return MenuItem::MENU_NONE;
}

void LevelMenuItem::draw(const GraphicsContext &gc)
{
	int timer = (parent->getCounter() - animStart) * 20;
//	textprintf_ex (gc.buffer, font, 0, 16 * level, BLACK, -1, "count %i, animating %i",
//			timer,
//			animating ? 1 : 0);

	int my = level / 2;
	int mx = level % 2;

	double tx = (gc.buffer->w / 2);
	double ty = (gc.buffer->h / 2);

	int w = anim->sizex;
	int h = anim->sizey;

	x = mx * tx + ((tx - w) / 2);
	y = my * ty + ((ty - h) / 2);

	// TODO: 20 is used to convert to msec. Check this.

	anim->drawFrame(gc.buffer, 0, 0,
			animating ? timer : 0 , x, y);

	int color = getColor();

	drawing_mode (DRAW_MODE_TRANS, NULL, 0, 0);
	set_trans_blender (0, 0, 0, 120);
	if (state.isLevelCleared(level))
	{
		rectfill (gc.buffer, x, y, x + w, y + h, BLUE);
	}
	if (isSelected() && !animating && parent->getCounter() % 40 > 20)
	{
		rectfill (gc.buffer, x, y, x + w, y + h, RED);
	}
	if (!state.isLevelEnabled(level))
	{
		rectfill (gc.buffer, x, y, x + w, y + h, GREY);
	}
	solid_mode();
}

string LevelMenuItem::getHint()
{
	return std::string("");
}

bool LevelMenuItem::isEnabled()
{
	return state.isLevelEnabled(level);
}
