#include <stdio.h>
#define ALLEGRO_STATICLINK
#include <allegro5/allegro.h>
#include <iostream>
#include <fstream>
#include <sstream>
//#include <allegro5/allegro_font.h>
//#include <allegro5/allegro_ttf.h>
#include "enemy.hpp"
#include <allegro5/allegro_image.h>
//#include "map.hpp"
#include "player.hpp"
#include "getValue.hpp"
//#include "animation.hpp"
//#include "animHandler.hpp"
//#include "attackHandler.hpp"
#include <string>
#include "map.hpp"
#include <vector>
//#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
//#include "hud.hpp"
#include "setControls.hpp"
#include "musicPlayer.hpp"'
#include "bullet.hpp"
#include <stdlib.h>
enum keyCodes {KEY_UP,KEY_DOWN,KEY_LEFT,KEY_RIGHT,KEY_SELECT,KEY_CANCEL};
enum currentMenu {MENU_TITLE_SCREEN,MENU_PLAYER_SELECT,MENU_LEVEL_SELECT};
const float FPS = 60;
const int SCREEN_W = 640;
const int SCREEN_H = 480;
const int BOUNCER_SIZE = 20;

int main(int argc, char **argv)
{
    int currentResW = 320;
    int currentResH = 240;
    ALLEGRO_DISPLAY *display = NULL;
    ALLEGRO_TIMER *timer = NULL;
    ALLEGRO_EVENT_QUEUE *event_queue = NULL;
    if(!al_init())
    {
        fprintf(stderr, "failed to initialize allegro!\n");
        return -1;
    }
    if(!al_init_image_addon())
    {
        fprintf(stderr, "failed to initialize the keyboard!\n");
        return -1;
    }
    if(!al_install_audio())
    {
        fprintf(stderr, "failed to initialize sound!\n");
        return -1;
    }
    if(!al_init_acodec_addon())
    {
        fprintf(stderr, "failed to initialize the sound loader!\n");
        return -1;
    }
    if(!al_reserve_samples(20))
    {
        fprintf(stderr, "failed to initialize samples!\n");
        return -1;
    }
    if(!al_install_keyboard())
    {
        fprintf(stderr, "failed to initialize the keyboard!\n");
        return -1;
    }
    event_queue = al_create_event_queue();
    if(!event_queue)
    {
        fprintf(stderr, "failed to create event_queue!\n");
        //al_destroy_bitmap(bouncer);
        al_destroy_display(display);
        return -1;
    }
    timer = al_create_timer(1.0 / FPS);
    if(!timer)
    {
        fprintf(stderr, "failed to create timer!\n");
        return -1;
    }
    display = al_create_display(SCREEN_W, SCREEN_H);
    if(!display)
    {
        fprintf(stderr, "failed to create display!\n");
        return -1;
    }
    float scaleX = al_get_display_width(display)/(float)currentResW;
    float scaleY = al_get_display_height(display)/(float)currentResH;
    ALLEGRO_TRANSFORM trans;
    al_identity_transform(&trans);
    al_scale_transform(&trans, scaleX, scaleY);
    al_use_transform(&trans);
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    bool quit = false;

    float initX = 200;
    float initY = 100;
    float trueY = 100;
    player player1(200,100);

    getValue numEnemies,enemyX,enemyY;
    std::string section = "tank";
    std::string iniFile = "enemy.ini";
    numEnemies.update("num",section,iniFile);
    std::vector<enemy> enemies;
    al_set_target_bitmap(al_get_backbuffer(display));
    ALLEGRO_BITMAP * tankSprite = al_load_bitmap("sheets/tank.tga");
    al_convert_mask_to_alpha(tankSprite,al_map_rgb(0,0,0));
    ALLEGRO_BITMAP * dragonSprite = al_load_bitmap("sheets/dragon.tga");
    al_convert_mask_to_alpha(dragonSprite,al_map_rgb(0,0,0));
    for(int i = 1; i <= numEnemies.numValue; i++)
    {
        std::stringstream out;
        out << i;
        std::string num = out.str();
        enemyX.update(num+"x",section,iniFile);
        enemyY.update(num+"y",section,iniFile);
        enemy newEnemy(enemyX.numValue, enemyY.numValue, 32, 16, tankSprite,140);
        enemies.push_back(newEnemy);
    }
    section = "dragon";
    numEnemies.update("num",section,iniFile);
    for(int i = 0; i < numEnemies.numValue; i++)
    {
        std::stringstream out;

        out << i+1;
        std::string num = out.str();
        enemyX.update(num+"x",section,iniFile);
        enemyY.update(num+"y",section,iniFile);
        enemy newEnemy(enemyX.numValue, enemyY.numValue, 32, 16, dragonSprite,80);
        enemies.push_back(newEnemy);
    }

    float bulletYOffset = 7;
    ALLEGRO_BITMAP *bulletSprite = al_load_bitmap("sheets/bullet.tga");
    al_convert_mask_to_alpha(bulletSprite,al_map_rgb(0,0,0));

    std::vector<bullet> bullets;
    std::vector<bullet> eBullets;

    musicPlayer mPlayer;
    mPlayer.init("inst.ini","song2",event_queue);
    mapLevel map1("map.ini","map1");
    al_set_target_bitmap(al_get_backbuffer(display));
    al_clear_to_color(al_map_rgb(0, 0, 255));
    mPlayer.start();
    al_start_timer(timer);
    al_flush_event_queue(event_queue);
    while(!quit)
    {
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);
        mPlayer.update(ev);
        if(ev.type == ALLEGRO_EVENT_KEY_DOWN or ev.type == ALLEGRO_EVENT_KEY_UP)
        {
            player1.update(ev,map1.getBlocks());
            if(ev.type == ALLEGRO_EVENT_KEY_DOWN and ev.keyboard.keycode == ALLEGRO_KEY_K)
            {
                player1.shooting = 15;
                if(player1.facingRight)
                {
                    bullet newBullet(player1.hitBox.getX() + player1.hitBox.getWidth(),player1.hitBox.getY() + bulletYOffset, 3, 3,bulletSprite);
                    newBullet.setMove(5,0);
                    bullets.push_back(newBullet);
                }else
                {
                    bullet newBullet(player1.hitBox.getX() - 3,player1.hitBox.getY() + bulletYOffset, 3, 3,bulletSprite);
                    newBullet.setMove(-5,0);
                    bullets.push_back(newBullet);
                }
            }
        }
        if(ev.type == ALLEGRO_EVENT_KEY_DOWN and ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
        {
            quit = true;
        }else if(ev.type == ALLEGRO_EVENT_TIMER)
        {
            if(ev.timer.source == timer)
            {
                if(trueY > 240.0)
                {
                    player1.hitBox.setX(initX);
                    player1.hitBox.setY(initY);
                }
                if(player1.update(ev,map1.getBlocks()))
                {
                    player1.hitBox.setX(0);
                    player1.hitBox.setY(0);
                }
                for(int i = 0; i < enemies.size(); i++)
                {
                    if(enemies.at(i).collide(player1.hitBox))
                    {
                        player1.hitBox.setX(0);
                        player1.hitBox.setY(0);
                    }
                    for(int j = 0; j < bullets.size(); j++)
                    {
                        if(enemies.at(i).collide(bullets.at(j)))
                        {
                            enemies.erase (enemies.begin()+i);
                        }
                    }
                }
                for(int i = 0; i < eBullets.size(); i++)
                {
                    if(eBullets.at(i).collide(player1.hitBox))
                    {
                        player1.hitBox.setX(0);
                        player1.hitBox.setY(0);
                    }
                }
                for(int i = 0; i < enemies.size(); i++)
                {
                    if(enemies.at(i).update())
                    {
                        bullet newBullet(enemies.at(i).getX() - 3,enemies.at(i).getY() + 4, 3, 3,bulletSprite);
                        newBullet.setMove(-5,0);
                        eBullets.push_back(newBullet);
                    }
                }
                for(int i = 0; i < bullets.size(); i++)
                {
                    bullets.at(i).update();
                    if(bullets.at(i).getX() > currentResW + 6 or bullets.at(i).getX() < -6)
                    {
                        bullets.erase (bullets.begin()+i);
                    }
                }
                for(int i = 0; i < eBullets.size(); i++)
                {
                    eBullets.at(i).update();
                    if(eBullets.at(i).getX() > currentResW + 6 or eBullets.at(i).getX() < -6)
                    {
                        eBullets.erase (eBullets.begin()+i);
                    }
                }
                float yScroll = 0;
                float xScroll = currentResW/2-player1.hitBox.getX();
                yScroll = currentResH/2-player1.hitBox.getY();
                initX += xScroll;
                initY += yScroll;
                trueY += -yScroll;
                for(int i = 0; i < bullets.size(); i++)
                {
                    bullets.at(i).scroll(xScroll,yScroll);
                }
                for(int i = 0; i < eBullets.size(); i++)
                {
                    eBullets.at(i).scroll(xScroll,yScroll);
                }
                map1.scroll(xScroll,yScroll);
                player1.scroll(xScroll,yScroll);
                for(int i = 0; i < enemies.size(); i++)
                {
                    enemies.at(i).scroll(xScroll,yScroll);
                }
                map1.draw();
                for(int i = 0; i < bullets.size(); i++)
                {
                    bullets.at(i).draw();
                }
                for(int i = 0; i < eBullets.size(); i++)
                {
                    eBullets.at(i).draw();
                }
                for(int i = 0; i < enemies.size(); i++)
                {
                    enemies.at(i).draw();
                }
                player1.draw();
                al_flip_display();
            }
        }
    }
    mPlayer.destroy();
    al_destroy_bitmap(bulletSprite);
    al_destroy_event_queue(event_queue);
    al_destroy_display(display);
}