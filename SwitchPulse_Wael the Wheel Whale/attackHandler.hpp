#ifndef life5_attackHandler_hpp
#define life5_attackHandler_hpp
#include <allegro5/allegro.h>
#include <string>
#include <vector>
#include <math.h>
#include "map.hpp"
#include "player.hpp"
#include "bullet.hpp"
#include "getValue.hpp"
#include "animHandler.hpp"
#include "animation.hpp"
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <stdio.h>
enum sounds {SOUNDS_BULLET_SHOOT,SOUNDS_BULLET_HIT,SOUNDS_KNIFE,SOUNDS_MINE_SET,SOUNDS_MINE_EXPLODE,SOUNDS_BUMP};
class attackHandler
{
    public:
    static void initHandler(animation * tempAnim,ALLEGRO_BITMAP * bitmap,ALLEGRO_BITMAP * bitmap2, std::string iniFile,std::string attackSection);
    static void addExplosion(float x, float y);
    static void updateAttacks(map & currentMap);
    static void reset();
    static void reset(player &playerToClear);
    static animation * explosionAnimation;
    static std::vector<animHandler> explosions;
    static std::vector<bullet> bullets, mines;
    static std::vector<float> explosionX,explosionY;
    static ALLEGRO_BITMAP * bulletBitmap;
    static ALLEGRO_BITMAP * mineBitmap;
    static void draw();
    static std::vector<ALLEGRO_SAMPLE *> sounds_;
};
#endif
