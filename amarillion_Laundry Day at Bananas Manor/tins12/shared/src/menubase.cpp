#include "menubase.h"
#include <allegro.h>
#include "color.h"
#include <alfont.h>
#include <assert.h>
#include "sound.h"
#include <stdio.h>

using namespace std;

void MenuItem::draw(const GraphicsContext &gc)
{
	assert (parent);

	int color = getColor();

#ifdef USE_ALFONT
	alfont_textout_centre_aa_ex (gc.buffer, parent->sfont, getText().c_str(), gc.buffer->w / 2, y,
		color, -1);
#else
	textout_centre_ex (gc.buffer, parent->sfont, getText().c_str(), gc.buffer->w / 2, y,
		color, -1);
#endif
}

int MenuItem::getColor()
{
	int color = parent->colorNormal;
	if (!enabled) color = parent->colorDisabled;
	if (flashing)
	{
		if ((parent->tFlash % 10) < 5) color = parent->colorFlash2;
	}
	else if (isSelected() && (parent->tFlash % 30) < 15) color = parent->colorFlash1;
	return color;
}

bool MenuItem::isSelected()
{
	return (*(parent->current))[parent->selected].get() == this;
}

int ActionMenuItem::handleInput()
{
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
				return action;
				break;
		}
	}
	return MenuItem::MENU_NONE;
}

int ToggleMenuItem::handleInput()
{
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
				toggle = !toggle;
				return action;
		}
	}
	return MenuItem::MENU_NONE;
}

ToggleMenuItem::ToggleMenuItem(MenuBase *parent, int _action, std::string _a, std::string _b, std::string _hint) :
		MenuItem(parent), action(_action), a(_a), b(_b), hint(_hint), toggle(false) {}

MenuBase::MenuBase(Sound *_sound) : selected (0), tFlash(0),
		current(), sound(_sound), inited(false), sound_enter(NULL), sound_cursor(NULL)
{
	topMargin = 60;
	bottomMargin = 100;
}

void MenuBase::init()
{
	//TODO: this may be wrong in case the buffer is not the same size as the screen resolution.
	// in that case you must call setLocation()
	w = SCREEN_W;
	h = SCREEN_H;
	colorNormal = YELLOW;
	colorFlash1 = RED;
	colorFlash2 = GREEN;
	colorDisabled = GREY;
	inited = true;

	hint = IComponentPtr(new Hint(this));
	add (hint);
}

void MenuBase::setMargins(int _top, int _bottom)
{
	topMargin = _top;
	bottomMargin = _bottom;
}

void MenuList::calculateLayout(int left, int top, int right, int bottom)
{
	std::vector<MenuItemPtr>::iterator i;
	int count = 0;

	for (i = items.begin(); i != items.end(); ++i)
		if ((*i)->isVisible()) count++;

	int y = top;
	int dy = (bottom - top) / (count + 2);
	for (i = items.begin(); i != items.end(); ++i)
	{
		if ((*i)->isVisible())
		{
			(*i)->setLocation(left, y, right - left, dy);
			y += dy;
		}
	}
}

void Hint::draw(const GraphicsContext &gc)
{
	MenuListPtr current = parent->getCurrent();
	assert (current);
	assert (sfont);
	int selected = parent->getSelectedIdx();
	string hint = (*current)[selected]->getHint();
#ifdef USE_ALFONT
	alfont_textout_centre_aa_ex (gc.buffer, sfont, hint.c_str(), gc.buffer->w / 2,
		y, WHITE, -1);
#else
	textout_centre_ex (gc.buffer, sfont, (*current)[selected]->getHint().c_str(), gc.buffer->w / 2,
		hinty, WHITE, -1);
#endif
}

int MenuBase::update()
{
	IContainer::update();
	assert(current);
	assert(inited);
	tFlash++;
	if (awake)
	{
		int action = (*current)[selected]->handleInput();
		switch (action)
		{
		case MenuItem::MENU_NEXT:
			next();
			break;
		case MenuItem::MENU_PREV:
			prev();
			break;
		case MenuItem::MENU_NONE:
			// do nothing.
			break;
		default:
			// let subclass handle action
			if (sound_enter && sound) sound->playSample (sound_enter);
			handleMessage (action);
			break;
		}
	}
	return 0;
}

void MenuBase::prev()
{
	assert (current);
	do
	{
		if (selected == 0) selected = current->size() - 1;
		else selected--;
	} while (!((*current)[selected]->isEnabled() &&
			(*current)[selected]->isVisible()));
	tFlash = 0;
	if (sound_cursor && sound) sound->playSample (sound_cursor);
}

void MenuBase::next()
{
	assert (current);
	do
	{
		if (++selected >= current->size()) selected = 0;
	} while (!((*current)[selected]->isEnabled() &&
			(*current)[selected]->isVisible()));
	tFlash = 0;
	if (sound_cursor && sound) sound->playSample (sound_cursor);
}

void MenuBase::setCurrent (MenuListPtr _current)
{
	//TODO: similar to focus mechanism. Merge.
	if (current)
	{
		current->setAwake (false);
		current->setVisible (false);
	}
	current = _current;
	current->setAwake (true);
	current->setVisible (true);
	selected = 0;
	tFlash = 0;
	current->calculateLayout(0, topMargin, w, h - bottomMargin);
	hint->sety (h - bottomMargin);
}

