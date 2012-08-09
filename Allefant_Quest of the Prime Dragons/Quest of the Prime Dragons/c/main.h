#ifndef _MAIN_
#define _MAIN_
typedef struct Application Application;
#include "common.h"
#include "raytrace.h"
#include "color.h"
#include "graphics.h"
#include "actor.h"
#include "item.h"
#include "parallax.h"
#include "wave.h"
#include "embed.h"
#include "special.h"
extern char * main_data_path;
extern LandFont * font;
struct Application {
    int w, h;
    int actors_yoffset;
    Actors * actors;
    float mx, my;
    float scroll;
    float parallax_scroll;
    bool paused;
};
extern Application * app;
void restart(void);
void save(void);
void load(void);
void runner_init(LandRunner * self);
void runner_done(LandRunner * self);
void runner_update(LandRunner * self);
void runner_redraw(LandRunner * self);
int my_main(void);
#endif
