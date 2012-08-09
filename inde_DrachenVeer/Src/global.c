//Source file containing globally linking variables.

#include <global.h>

#ifdef IPHONE
#define XRES 320
#define YRES 480
#else
#define XRES 640
#define YRES 480
#endif

int xres = XRES;
int yres = YRES;
int windowed_mode = 1;
int display_flags = 0;
int lps = 64;
int soundsystem = 0;
int mouse = 0;
int touch = 0;
ALLEGRO_BITMAP *icon = NULL;
ALLEGRO_BITMAP *rbuffer = NULL;
ALLEGRO_DISPLAY *screen = NULL;
