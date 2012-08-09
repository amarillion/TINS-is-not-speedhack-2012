#include <assert.h>
#include "settings.h"
#include <allegro.h>
#include <string.h>

const char *config_keys[NUM_BUTTONS] = 
{
	"key_up", "key_down", "key_left", "key_right", "key_action"
};

Settings::Settings()
{
	button[btnUp].setScancode (KEY_UP);
	button[btnDown].setScancode (KEY_DOWN);
	button[btnLeft].setScancode (KEY_LEFT);
	button[btnRight].setScancode (KEY_RIGHT);
	button[btnAction].setScancode (KEY_SPACE);
	button[btnAction].setAltcode (KEY_ENTER);	
}

void Settings::getFromConfig()
{
	for (int i = 0; i < NUM_BUTTONS; ++i)
	{
		button[i].setScancode (get_config_int ("tins10", config_keys[i],
			button[i].getScancode()));
	}
}	
