#ifndef MAIN_H
#define MAIN_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <vector>

#include "leveldata.h"

extern ALLEGRO_DISPLAY *display;
extern ALLEGRO_EVENT_QUEUE *queue;
extern ALLEGRO_TIMER *timer;
extern ALLEGRO_PATH *path;
extern bool hiRes;
extern LevelData levels[23];
extern int selectedLevel;
extern int maxLevel;
extern const int totalLevels;

#endif
