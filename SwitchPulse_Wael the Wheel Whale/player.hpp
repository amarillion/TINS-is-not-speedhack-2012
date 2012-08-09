#ifndef life5_player_hpp
#define life5_player_hpp
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include "block.hpp"
#include "map.hpp"
#include <string>
#include "animation.hpp"
#include "animHandler.hpp"
#include "getValue.hpp"
#include <stdio.h>
#include <allegro5/allegro_primitives.h>
#include <math.h>
enum waelStates{RUNNING,JUMPING,STANDING};
class player
{
    public:
    player(float x, float y)
    {
        block newBlock(x,y,32,16);
        hitBox = newBlock;
        facingRight = true;
        left = false;
        right = false;
        jump = false;
        air = 0;
        floor = false;
        shooting=0;
        currentAnim=1;
        animation tempAnim(al_load_bitmap("sheets/bodywalk.tga"),-8,-4,48, 20,false,1);
        anims.push_back(tempAnim);
        animation tempAnim2(al_load_bitmap("sheets/bodyjump1.tga"),-14,-14,52, 40,false,0);
        anims.push_back(tempAnim2);
        myHandler.setHandler(anims.at(1));
        heads = al_load_bitmap("sheets/heads.tga");
        al_convert_mask_to_alpha(heads,al_map_rgb(0,0,0));
    }
    bool update(ALLEGRO_EVENT ev, std::vector<block> map);
    void draw();
    void scroll(float xScroll, float yScroll);
    block hitBox;
    //ALLEGRO_BITMAP * sprite;
    bool left;
    bool right;
    animHandler myHandler;
    bool jump;
    bool facingRight;
    int shooting;
    float air;
    bool floor;
    int currentAnim;
    std::vector<animation> anims;
    ALLEGRO_BITMAP * heads;
};
#endif
