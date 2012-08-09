#ifndef _WAVE_
#define _WAVE_
#include "actor.h"
extern int wave_ticks;
void wave_restart(void);
char const * wave_info(void);
void wave_tick(void);
#endif
