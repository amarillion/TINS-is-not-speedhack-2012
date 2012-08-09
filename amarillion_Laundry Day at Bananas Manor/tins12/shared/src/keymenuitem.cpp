#include "keymenuitem.h"
#include <allegro.h>

using namespace std;

int KeyMenuItem::handleInput()
{
	if (waitForKey)
	{
		int c = getLastScancode();
		if (c != 0)
		{
			clear_keybuf();
			if (c == KEY_ESC)
			{
			}
			else
			{
				btn.setScancode(c);
				set_config_int ("twist", btnConfigName, c);
			}
			waitForKey = false;
			flashing = false;
		}
	}
	else
	{
		if (keypressed())
		{
			int c = readkey();
			switch (c >> 8)
			{
				case KEY_LEFT:
				case KEY_UP:
					return MenuItem::MENU_PREV;
					break;
				case KEY_DOWN:
				case KEY_RIGHT:
					return MenuItem::MENU_NEXT;
					break;
				case KEY_ENTER:
				case KEY_SPACE:
					resetLastScancode();
					waitForKey = true;
					flashing = true;
					break;
			}
		}
	}

	return MenuItem::MENU_NONE;
}

string KeyMenuItem::getText()
{
	return string (btnName + " [" + btn.name() + "]");
}

string KeyMenuItem::getHint()
{
	if (waitForKey)
		return string ("press a new key or esc to cancel");
	else
		return string ("press enter to change key");
}

