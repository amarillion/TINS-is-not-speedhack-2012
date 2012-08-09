#ifndef MENUBASE_H
#define MENUBASE_H

#include <string>
#include <vector>
#include "component.h"
#include <boost/shared_ptr.hpp>
#include "IContainer.h"

struct ALFONT_FONT;
struct BITMAP;
struct SAMPLE;
struct FONT;

class Sound;

class MenuBase;

class MenuItem : public IComponent
{
	protected:
		MenuBase *parent;
		bool flashing;
		bool enabled;
	public:
		enum { MENU_NONE = 1000, MENU_PREV, MENU_NEXT };
		MenuItem (MenuBase *_parent) : parent(_parent), flashing (false), enabled(true) {}
		virtual ~MenuItem() {}
		bool isSelected();
		virtual int handleInput() = 0;
		virtual std::string getText() = 0;
		virtual std::string getHint() = 0;
		virtual void draw(const GraphicsContext &gc);
		void setEnabled (bool value) { enabled = value; }
		virtual bool isEnabled() { return enabled; }

		int getColor();
};

class ActionMenuItem : public MenuItem
{
	private:
		int action; // code returned from handleInput
		std::string text;
		std::string hint;
	public:
		ActionMenuItem (MenuBase *parent, int _action, std::string _text, std::string _hint) :
			MenuItem(parent), action (_action), text (_text), hint (_hint) {}
		virtual int handleInput();
		virtual std::string getText() { return text; }
		virtual std::string getHint() { return hint; }
		void setText (std::string value) { text = value; }
};

class ToggleMenuItem : public MenuItem
{
private:
	int action;
	std::string a;
	std::string b;
	std::string hint;
	bool toggle;
public:
	ToggleMenuItem(MenuBase *parent, int _action, std::string _a, std::string _b, std::string _hint);
	void setToggle(bool value) { toggle = value; }
	bool getToggle() { return toggle; }
	virtual int handleInput();
	virtual std::string getText() { return toggle ? a : b; }
	virtual std::string getHint() { return hint; }
};

typedef boost::shared_ptr<MenuItem> MenuItemPtr;

class MenuList : public IContainer
{
private:
	std::vector<MenuItemPtr> items;
public:
	MenuList() { awake = false; visible = false; }
	virtual ~MenuList() {}
	unsigned int size () { return items.size(); }
	void push_back(const MenuItemPtr &item)
	{
		// add both to items and children.
		items.push_back(item);
		add (boost::shared_dynamic_cast<IComponent>(item));
	}
	MenuItemPtr& operator[] (int idx) { return items[idx]; }
	void calculateLayout(int left, int top, int right, int bottom);
};

typedef boost::shared_ptr<MenuList> MenuListPtr;

class MenuBase : public IContainer
{
private:
	friend class MenuItem;
	void prev();
	void next();
	unsigned int selected;
	int tFlash;
	MenuListPtr current;
	Sound *sound;
	int topMargin;
	int bottomMargin;
	bool inited;
	IComponentPtr hint;
protected:
	SAMPLE *sound_enter;
	SAMPLE *sound_cursor;
	int colorNormal;
	int colorFlash1;
	int colorFlash2;
	int colorDisabled;
	void setCurrent (MenuListPtr _current);
public:
	MenuBase(Sound *_sound = NULL);
	virtual int update();
	virtual void init();
	void setMargins(int _top, int _bottom);
	int getSelectedIdx() { return selected; }
	MenuItemPtr getSelectedItem() { return (*current)[selected]; }
	MenuListPtr getCurrent() { return current; }
};

class MenuBuilder
{
public:
	MenuBuilder(MenuBase *parent)
	{
		result = MenuListPtr(new MenuList());
		parent->add(result, false, -1);
	}

	MenuBuilder &push_back (const MenuItemPtr &item)
	{
		result->push_back(item);
		return *this;
	}

	MenuListPtr build () { return result; }

private:
	MenuListPtr result;
};

class Hint : public IComponent
{
private:
	MenuBase *parent;
public:
	Hint(MenuBase *parent) { this->parent = parent; }
	virtual void draw (const GraphicsContext &gc);
};

#endif
