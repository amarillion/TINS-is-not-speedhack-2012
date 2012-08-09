#ifndef life5_hud_hpp
#define life5_hud_hpp
#include <string>
#include <vector>
#include <allegro5/allegro.h>
#include <stdio.h>
#include "getValue.hpp"
#include "map.hpp"
#include "player.hpp"
#include <allegro5/allegro_primitives.h>
class hud
{
    public:
    static void initHud(std::string iniFile,std::string hudSection, map& tempMap);
    static void draw(int screenWidth);
    private:
    static ALLEGRO_COLOR player1Color_;
    static ALLEGRO_COLOR player2Color_;
    static player * player1_;
    static player * player2_;
    static int height_, y_;
};
#endif
