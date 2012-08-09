#ifndef _GLOBAL_H_
#define _GLOBAL_H_

//Header file which gives a global variable list one can use to compile with. 

#include <header.h>
#define SEC_TO_LPS(x) ((int)((x)*lps))
#define MSEC_TO_LPS(x) ((int)(SEC_TO_LPS(x)/1000))

#define DATADIR "Data"
#define ICON "icon.bmp"

extern int xres;
extern int yres;
extern int windowed_mode;
extern int display_flags;
extern int lps;
extern int soundsystem;
extern int mouse;
extern int touch;
extern ALLEGRO_BITMAP *icon;
extern ALLEGRO_BITMAP *rbuffer;
extern ALLEGRO_DISPLAY *screen;

#endif //_GLOBAL_H_
