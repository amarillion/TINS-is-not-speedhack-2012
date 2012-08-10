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
    al_set_target_bitmap(al_get_backbuffer(display));

    float initX = 200;
    float initY = 100;
    float trueY = 100;
    player player1(200,100);

    getValue numEnemies,enemyX,enemyY;
    std::string section = "tank";
    std::string iniFile = "enemy.ini";
    numEnemies.update("num",section,iniFile);
    std::vector<enemy> enemies;
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
//    int currentResW = 320;
//    int currentResH = 240;
//    ALLEGRO_DISPLAY *display = NULL;
//    ALLEGRO_EVENT_QUEUE *event_queue = NULL;
//    ALLEGRO_TIMER *timer = NULL;
//    ALLEGRO_BITMAP *bouncer = NULL;
//    ALLEGRO_BITMAP *bouncer2 = NULL;
//    ALLEGRO_BITMAP *bulletBitmap = NULL;
//    ALLEGRO_BITMAP *mineBitmap = NULL;
//    ALLEGRO_BITMAP *blockBitmap = NULL;
//    ALLEGRO_BITMAP *someBitmap = NULL;
//    ALLEGRO_FONT *font = NULL;
//    ALLEGRO_FONT *font2 = NULL;
//    ALLEGRO_SAMPLE * gameSong = NULL;
//    bool redraw = true;
//    bool doExit = false;
//    if(!al_init())
//    {
//        fprintf(stderr, "failed to initialize allegro!\n");
//        return -1;
//    }
//    if(!al_init_primitives_addon())
//    {
//        fprintf(stderr, "failed to initialize primitives!\n");
//        return -1;
//    }
//    al_init_font_addon();
//    if(!al_install_audio())
//    {
//        fprintf(stderr, "failed to initialize sound!\n");
//        return -1;
//    }
//    if(!al_init_acodec_addon())
//    {
//        fprintf(stderr, "failed to initialize the sound loader!\n");
//        return -1;
//    }
//    if(!al_reserve_samples(10))
//    {
//        fprintf(stderr, "failed to initialize samples!\n");
//        return -1;
//    }
//    if(!al_init_ttf_addon())
//    {
//        fprintf(stderr, "failed to initialize fonts!\n");
//        return -1;
//    }
//    if(!al_install_keyboard())
//    {
//        fprintf(stderr, "failed to initialize the keyboard!\n");
//        return -1;
//    }
//    if(!al_init_image_addon())
//    {
//        fprintf(stderr, "failed to initialize the keyboard!\n");
//        return -1;
//    }
//    timer = al_create_timer(1.0 / FPS);
//    if(!timer)
//    {
//        fprintf(stderr, "failed to create timer!\n");
//        return -1;
//    }
//    display = al_create_display(SCREEN_W, SCREEN_H);
//    if(!display)
//    {
//        fprintf(stderr, "failed to create display!\n");
//        al_destroy_timer(timer);
//        return -1;
//    }
//    al_set_window_title(display, "jumpTheGunBETA");
//
//    ALLEGRO_BITMAP *cursor1 = al_load_bitmap("settings/cursor-blue.tga");
//    ALLEGRO_BITMAP *preLoad = al_load_bitmap("settings/preLoad.tga");
//    al_set_display_icon(display, cursor1);
//    font = al_load_ttf_font("settings/silkscreen.ttf",10,ALLEGRO_TTF_MONOCHROME);
//    font2 = al_load_ttf_font("settings/silkscreen.ttf",20,ALLEGRO_TTF_MONOCHROME);
//    bouncer = al_create_bitmap(24,15);
//    bouncer2 = al_create_bitmap(24,15);
//    mineBitmap = al_create_bitmap(4, 2);
//    bulletBitmap = al_create_bitmap(2, 2);
//    if(!bouncer)
//    {
//        fprintf(stderr, "failed to create bouncer bitmap!c\n");
//        al_destroy_display(display);
//        al_destroy_timer(timer);
//        return -1;
//    }
//    if(!bouncer2)
//    {
//        fprintf(stderr, "failed to create bouncer bitmap!c\n");
//        al_destroy_display(display);
//        al_destroy_timer(timer);
//        return -1;
//    }
//    al_set_target_bitmap(bouncer);
//    al_clear_to_color(al_map_rgb(0, 255, 45));
//    al_set_target_bitmap(bouncer2);
//    al_clear_to_color(al_map_rgb(255, 0, 0));
//    al_set_target_bitmap(bulletBitmap);
//    al_clear_to_color(al_map_rgb(0,0,0));
//    al_set_target_bitmap(mineBitmap);
//    al_clear_to_color(al_map_rgb(0,0,0));
//    blockBitmap = al_create_bitmap(BOUNCER_SIZE, BOUNCER_SIZE);
//    if(!blockBitmap)
//    {
//        fprintf(stderr, "failed to create block bitmap!\n");
//        al_destroy_display(display);
//        al_destroy_timer(timer);
//        return -1;
//    }
//    al_set_target_bitmap(blockBitmap);
//    al_clear_to_color(al_map_rgb(0, 0, 255));
//    al_set_target_bitmap(al_get_backbuffer(display));
//
//    event_queue = al_create_event_queue();
//    if(!event_queue)
//    {
//        fprintf(stderr, "failed to create event_queue!\n");
//        al_destroy_bitmap(bouncer);
//        al_destroy_display(display);
//        al_destroy_timer(timer);
//        return -1;
//    }
//
//    al_register_event_source(event_queue, al_get_display_event_source(display));
//
//    float scaleX = al_get_display_width(display)/(float)currentResW;
//    float scaleY = al_get_display_height(display)/(float)currentResH;
//    ALLEGRO_TRANSFORM trans;
//    al_identity_transform(&trans);
//    al_scale_transform(&trans, scaleX, scaleY);
//    al_use_transform(&trans);
//    al_clear_to_color(al_map_rgb(0,0,0));
//    ALLEGRO_BITMAP *titleLogo = al_load_bitmap("settings/titleLogo.tga");
//    al_draw_bitmap(titleLogo,0,0,0);
//    al_draw_textf(font, al_map_rgb(255,255,255),currentResW - 10,currentResH - 60, ALLEGRO_ALIGN_RIGHT,"Loading...");
//    al_flip_display();
//    std::vector<std::string> names;
//    names.push_back("up");
//    names.push_back("down");
//    names.push_back("left");
//    names.push_back("right");
//    names.push_back("jump");
//    std::vector<std::string> sections;
//    sections.push_back("player1");
//    sections.push_back("player2");
//    std::string iniFile = "settings/controls.ini";
//    std::string p1Controls = "player1";
//    std::string p2Controls = "player2";
//    std::string attackSettings = "soundEffects";
//    std::string hudSettings = "hud";
//    std::vector<int> keys;
//    keys.push_back(ALLEGRO_KEY_A);
//    keys.push_back(ALLEGRO_KEY_D);
//    keys.push_back(ALLEGRO_KEY_W);
//    keys.push_back(ALLEGRO_KEY_S);
//    keys.push_back(ALLEGRO_KEY_G);
//    keys.push_back(ALLEGRO_KEY_J);
//    keys.push_back(ALLEGRO_KEY_L);
//    keys.push_back(ALLEGRO_KEY_I);
//    keys.push_back(ALLEGRO_KEY_K);
//    keys.push_back(ALLEGRO_KEY_ENTER);
//    for(int i = 0;i < sections.size();++i)
//    {
//        for(int j = 0;j < names.size();++j)
//        {
//            int tempInt = i*names.size() + j;
//            saveValue(ALLEGRO_KEY_P,"fire","player2",iniFile);
//        }
//    }
//    al_register_event_source(event_queue, al_get_timer_event_source(timer));
//    al_register_event_source(event_queue, al_get_keyboard_event_source());
//    al_start_timer(timer);
//    std::vector<std::string> playerSelection,playerSelectionGraphics,maps, mapNames;
//    std::vector<ALLEGRO_BITMAP *> mapPreviews;
//    playerSelection.push_back("physics1");
//    playerSelection.push_back("physics2");
//    playerSelection.push_back("physics3");
//    playerSelectionGraphics.push_back("gunner");
//    playerSelectionGraphics.push_back("ninja");
//    playerSelectionGraphics.push_back("pyro");
//    maps.push_back("map1");
//    maps.push_back("map2");
//    maps.push_back("map3");
//    mapNames.push_back("Chamber");
//    mapNames.push_back("Ruins");
//    mapNames.push_back("Tundra");
//    al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR);
//    for(int i = 0; i < maps.size(); ++i)
//    {
//        std::string tempString = maps.at(i);
//        tempString.append("Preview");
//        getValue bitmapText(tempString.data(),"maps",iniFile);
//        mapPreviews.push_back(al_load_bitmap(bitmapText.stringValue.data()));
//        if(!mapPreviews.at(i))
//        {
//            fprintf(stderr, "failed to create bouncer bitmap! %s %s\n", tempString.data(),bitmapText.stringValue.data());
//            exit(EXIT_FAILURE);
//        }
//    }
//    int characterSelectionPlayer1, characterSelectionPlayer2;
//    int menu = MENU_TITLE_SCREEN;
//    int currentMapInt;
//    al_flush_event_queue(event_queue);
//    ALLEGRO_SAMPLE *introSong = al_load_sample("settings/introSong.ogg");
//    ALLEGRO_SAMPLE *victorySong = al_load_sample("settings/victorySong.ogg");
//    ALLEGRO_SAMPLE *selectSFX = al_load_sample("settings/select.ogg");
//    ALLEGRO_SAMPLE *chooseSFX = al_load_sample("settings/choose.wav");
//    ALLEGRO_SAMPLE *death = al_load_sample("settings/death.ogg");
//    ALLEGRO_SAMPLE *cancelSFX = al_load_sample("settings/cancel.wav");
//    ALLEGRO_SAMPLE *countDownSound = al_load_sample("settings/countDown.wav");
//    ALLEGRO_SAMPLE *goSound = al_load_sample("settings/go.wav");
//    ALLEGRO_SAMPLE_ID introSongID;
//    ALLEGRO_BITMAP *cursor2 = al_load_bitmap("settings/cursor-red.tga");
//    ALLEGRO_BITMAP *cursorB = al_load_bitmap("settings/cursor-highlight-blue.tga");
//    ALLEGRO_BITMAP *cursor2B = al_load_bitmap("settings/cursor-highlight-red.tga");
//    ALLEGRO_BITMAP *gunnerBluePose = al_load_bitmap("settings/gunner-pose.tga");
//    ALLEGRO_BITMAP *gunnerRedPose = al_load_bitmap("settings/gunner-pose-red.tga");
//    ALLEGRO_BITMAP *ninjaBluePose = al_load_bitmap("settings/ninja-pose-blue.tga");
//    ALLEGRO_BITMAP *ninjaRedPose = al_load_bitmap("settings/ninja-pose-red.tga");
//    ALLEGRO_BITMAP *pyroBluePose = al_load_bitmap("settings/pyro-pose-blue.tga");
//    ALLEGRO_BITMAP *pyroRedPose = al_load_bitmap("settings/pyro-pose-red.tga");
//    ALLEGRO_BITMAP *gunnerBlueVS = al_load_bitmap("settings/gunner-vs-blue.tga");
//    ALLEGRO_BITMAP *gunnerRedVS = al_load_bitmap("settings/gunner-vs-red.tga");
//    ALLEGRO_BITMAP *ninjaBlueVS = al_load_bitmap("settings/ninja-vs-blue.tga");
//    ALLEGRO_BITMAP *ninjaRedVS = al_load_bitmap("settings/ninja-vs-red.tga");
//    ALLEGRO_BITMAP *pyroBlueVS = al_load_bitmap("settings/pyro-vs-blue.tga");
//    ALLEGRO_BITMAP *pyroRedVS = al_load_bitmap("settings/pyro-vs-red.tga");
//    ALLEGRO_BITMAP *screenFlash = al_load_bitmap("settings/screenFlash.tga");
//    ALLEGRO_BITMAP *gunnerStats = al_load_bitmap("settings/gunner-stats.tga");
//    ALLEGRO_BITMAP *ninjaStats = al_load_bitmap("settings/ninja-stats.tga");
//    ALLEGRO_BITMAP *pyroStats = al_load_bitmap("settings/pyro-stats.tga");
//    ALLEGRO_BITMAP *mineExplosion = al_load_bitmap("settings/splosion.tga");
//    animation screenFlashAnim(screenFlash,0,0,currentResW,currentResH,false,5);
//    gameSong = al_load_sample("settings/mainSong.ogg");
//    ALLEGRO_SAMPLE * versusSong = al_load_sample("settings/versusScreen.ogg");
//    bool quit = false;
//    al_set_target_bitmap(al_get_backbuffer(display));
//    while(!quit)
//    {
//        bool classicMode = false;
//        al_stop_samples();
//        al_play_sample(introSong, 2.7, 0, 1,ALLEGRO_PLAYMODE_LOOP, &introSongID);
//        while(!doExit)
//        {
//            bool selected = false;
//            bool exited = false;
//            bool selected2 = false;
//            bool exited2 = false;
//            int currentMenuSelection = 0;
//            int currentMenuSelection2 = 0;
//            std::vector<int> p1Keys,p2Keys;
//            std::vector<std::string> intsToGet;
//            intsToGet.push_back("up");
//            intsToGet.push_back("down");
//            intsToGet.push_back("left");
//            intsToGet.push_back("right");
//            intsToGet.push_back("jump");
//            intsToGet.push_back("fire");
//            getValue value;
//            for(int i = 0; i<intsToGet.size(); ++i)
//            {
//                value.update(intsToGet.at(i),p1Controls,iniFile);
//                if(!value.isNumber)
//                {
//                    fprintf(stderr, "Recieved string value where number was expected: Name: %s\n",intsToGet.at(i).data());
//                    exit(EXIT_FAILURE);
//                }
//                p1Keys.push_back(value.numValue);
//
//                value.update(intsToGet.at(i),p2Controls,iniFile);
//                if(!value.isNumber)
//                {
//                    fprintf(stderr, "Recieved string value where number was expected: Name: %s\n",intsToGet.at(i).data());
//                    exit(EXIT_FAILURE);
//                }
//                p2Keys.push_back(value.numValue);
//            }
//            bool done=false;
//            al_flush_event_queue(event_queue);
//            while(!done)
//            {
//                ALLEGRO_EVENT ev;
//                al_wait_for_event(event_queue, &ev);
//                if(ev.type == ALLEGRO_EVENT_KEY_DOWN)
//                {
//                    if((ev.keyboard.keycode == p1Keys.at(KEY_UP) or ev.keyboard.keycode == p1Keys.at(KEY_LEFT)) and !selected)
//                    {
//                        --currentMenuSelection;
//                        al_play_sample(selectSFX, 1, 0, 1,ALLEGRO_PLAYMODE_ONCE, NULL);
//                    }
//                    else if((ev.keyboard.keycode == p1Keys.at(KEY_DOWN) or ev.keyboard.keycode == p1Keys.at(KEY_RIGHT)) and !selected)
//                    {
//                        ++currentMenuSelection;
//                        al_play_sample(selectSFX, 1, 0, 1,ALLEGRO_PLAYMODE_ONCE, NULL);
//                    }
//                    else if((ev.keyboard.keycode == p2Keys.at(KEY_UP) or ev.keyboard.keycode == p2Keys.at(KEY_LEFT)) and !selected2)
//                    {
//                        --currentMenuSelection2;
//                        al_play_sample(selectSFX, 1, 0, 1,ALLEGRO_PLAYMODE_ONCE, NULL);
//                    }
//                    else if((ev.keyboard.keycode == p2Keys.at(KEY_DOWN) or ev.keyboard.keycode == p2Keys.at(KEY_RIGHT)) and !selected2)
//                    {
//                        ++currentMenuSelection2;
//                        al_play_sample(selectSFX, 1, 0, 1,ALLEGRO_PLAYMODE_ONCE, NULL);
//                    }
//                    else if(ev.keyboard.keycode == p1Keys.at(KEY_SELECT) and !selected)
//                    {
//                        selected = true;
//                        al_play_sample(chooseSFX, 1, 0, 1,ALLEGRO_PLAYMODE_ONCE, NULL);
//                    }
//                    else if(ev.keyboard.keycode == p2Keys.at(KEY_SELECT) and !selected2)
//                    {
//                        selected2 = true;
//                        al_play_sample(chooseSFX, 1, 0, 1,ALLEGRO_PLAYMODE_ONCE, NULL);
//                    }
//                    else if(ev.keyboard.keycode == p1Keys.at(KEY_CANCEL)and !exited)
//                    {
//                        exited = true;
//                        al_play_sample(cancelSFX, 1, 0, 1,ALLEGRO_PLAYMODE_ONCE, NULL);
//                    }
//                    else if(ev.keyboard.keycode == p2Keys.at(KEY_CANCEL)and !exited2)
//                    {
//                        exited2 = true;
//                        al_play_sample(cancelSFX, 1, 0, 1,ALLEGRO_PLAYMODE_ONCE, NULL);
//                    }
//                    else if(ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
//                    {
//                        doExit = true;
//                        quit = true;
//                        done = true;
//                    }
//                    else if(ev.keyboard.keycode == ALLEGRO_KEY_UP)
//                    {
//                        if(menu == MENU_TITLE_SCREEN)
//                        {
//                            --currentMenuSelection;
//                        }
//                    }
//                    else if(ev.keyboard.keycode == ALLEGRO_KEY_DOWN)
//                    {
//                        if(menu == MENU_TITLE_SCREEN)
//                        {
//                            ++currentMenuSelection;
//                        }
//                    }
//                    else if(ev.keyboard.keycode == ALLEGRO_KEY_ENTER)
//                    {
//                        if(menu == MENU_TITLE_SCREEN)
//                        {
//                            selected = true;
//                        }
//                    }
//                }
//                if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
//                {
//                    doExit = true;
//                    quit = true;
//                    done = true;
//                }
//                if(menu == MENU_TITLE_SCREEN)
//                {
//                    currentMenuSelection += currentMenuSelection2;
//                    currentMenuSelection2 = 0;
//                    if(currentMenuSelection > 2)
//                    {
//                        currentMenuSelection = 0;
//                    }
//                    if(currentMenuSelection < 0)
//                    {
//                        currentMenuSelection = 1;
//                    }
//                    if(selected or selected2)
//                    {
//                        if(currentMenuSelection == 0)
//                        {
//                            selected = false;
//                            selected2 = false;
//                            exited = false;
//                            exited2 = false;
//                            currentMenuSelection = 0;
//                            currentMenuSelection2 = 0;
//                            menu = MENU_PLAYER_SELECT;
//                        }else if(currentMenuSelection == 2)
//                        {
//                            setControls(iniFile,p1Controls,p2Controls,font,currentResW/2,currentResH/2);
//                            done = true;
//                            al_clear_to_color(al_map_rgb(0,0,0));
//                            al_flip_display();
//                        }else
//                        {
//                            classicMode = true;
//                            selected = false;
//                            selected2 = false;
//                            exited = false;
//                            exited2 = false;
//                            currentMenuSelection = 0;
//                            currentMenuSelection2 = 0;
//                            menu = MENU_PLAYER_SELECT;
//                        }
//                    }
//                    if(menu == MENU_TITLE_SCREEN and !done)
//                    {
//                        al_clear_to_color(al_map_rgb(0,0,0));
//                        al_draw_bitmap(cursor1,currentResW - 113, currentMenuSelection*21 + currentResH - 75,0);
//                        al_draw_bitmap(titleLogo,0,0,0);
//                        al_draw_textf(font, al_map_rgb(255,255,255),currentResW - 14,currentResH - 75, ALLEGRO_ALIGN_RIGHT,"NORMAL MODE");
//                        al_draw_textf(font, al_map_rgb(255,255,255),currentResW - 14,currentResH - 55, ALLEGRO_ALIGN_RIGHT,"CLASSIC MODE");
//                        al_draw_textf(font, al_map_rgb(255,255,255),currentResW - 14,currentResH - 35, ALLEGRO_ALIGN_RIGHT,"SET CONTROLS");
//                        al_flip_display();
//                    }
//                }
//                else if(menu == MENU_PLAYER_SELECT)
//                {
//                    if(currentMenuSelection < 0)
//                    {
//                        currentMenuSelection = playerSelection.size() - 1;
//                    }
//                    else if(currentMenuSelection == playerSelection.size())
//                    {
//                        currentMenuSelection = 0;
//                    }
//
//                    if(currentMenuSelection2 < 0)
//                    {
//                        currentMenuSelection2 = playerSelection.size() - 1;
//                    }
//                    else if(currentMenuSelection2 >= playerSelection.size())
//                    {
//                        currentMenuSelection2 = 0;
//                    }
//                    if(selected and selected2)
//                    {
//                        characterSelectionPlayer1 = currentMenuSelection;
//                        characterSelectionPlayer2 = currentMenuSelection2;
//                        selected = false;
//                        selected2 = false;
//                        exited = false;
//                        exited2 = false;
//                        currentMenuSelection = 0;
//                        currentMenuSelection2 = 0;
//                        menu = MENU_LEVEL_SELECT;
//                    }
//                    if(exited)
//                    {
//                        if(selected)
//                        {
//                            selected = false;
//                        }
//                        else
//                        {
//                            selected = false;
//                            selected2 = false;
//                            exited = false;
//                            exited2 = false;
//                            currentMenuSelection = 0;
//                            currentMenuSelection2 = 0;
//                            menu = MENU_TITLE_SCREEN;
//                        }
//                        exited = false;
//                    }
//                    if(exited2)
//                    {
//                        if(selected2)
//                        {
//                            selected2 = false;
//                        }
//                        else
//                        {
//                            selected = false;
//                            selected2 = false;
//                            exited = false;
//                            exited2 = false;
//                            currentMenuSelection = 0;
//                            currentMenuSelection2 = 0;
//                            menu = MENU_TITLE_SCREEN;
//                        }
//                        exited2 = false;
//                    }
//                    if(menu == MENU_PLAYER_SELECT)
//                    {
//                        al_clear_to_color(al_map_rgb(0,0,0));
//                        if(currentMenuSelection == 0)
//                        {
//                            al_draw_scaled_bitmap(gunnerBluePose,0,0, al_get_bitmap_width(gunnerBluePose), al_get_bitmap_height(gunnerBluePose),30, 50, 88, 88, 0);
//                            al_draw_scaled_bitmap(gunnerStats,0,0, al_get_bitmap_width(gunnerStats), al_get_bitmap_height(gunnerStats),34, 140, 80, 80, 0);
//                        }
//                        else if(currentMenuSelection == 1)
//                        {
//                            al_draw_scaled_bitmap(ninjaBluePose,0,0, al_get_bitmap_width(ninjaBluePose), al_get_bitmap_height(ninjaBluePose),30, 50, 88, 88, 0);
//                            al_draw_scaled_bitmap(ninjaStats,0,0, al_get_bitmap_width(ninjaStats), al_get_bitmap_height(ninjaStats),34, 140, 80, 80, 0);
//                        }
//                        else if(currentMenuSelection == 2)
//                        {
//                            al_draw_scaled_bitmap(pyroBluePose,0,0, al_get_bitmap_width(pyroBluePose), al_get_bitmap_height(pyroBluePose),30, 50, 88, 88, 0);
//                            al_draw_scaled_bitmap(pyroStats,0,0, al_get_bitmap_width(pyroStats), al_get_bitmap_height(pyroStats),34, 140, 80, 80, 0);
//                        }
//                        if(currentMenuSelection2 == 0)
//                        {
//                            al_draw_scaled_bitmap(gunnerRedPose,0,0, al_get_bitmap_width(gunnerRedPose), al_get_bitmap_height(gunnerRedPose),currentResW - 120, 50, 88, 88, 0);
//                            al_draw_scaled_bitmap(gunnerStats,0,0, al_get_bitmap_width(gunnerStats), al_get_bitmap_height(gunnerStats),currentResW - 112, 140, 80, 80, 0);
//                        }
//                        else if(currentMenuSelection2 == 1)
//                        {
//                            al_draw_scaled_bitmap(ninjaRedPose,0,0, al_get_bitmap_width(ninjaRedPose), al_get_bitmap_height(ninjaRedPose),currentResW - 120, 50, 88, 88, 0);
//                            al_draw_scaled_bitmap(ninjaStats,0,0, al_get_bitmap_width(ninjaStats), al_get_bitmap_height(ninjaStats),currentResW - 112, 140, 80, 80, 0);
//                        }
//                        else if(currentMenuSelection2 == 2)
//                        {
//                            al_draw_scaled_bitmap(pyroRedPose,0,0, al_get_bitmap_width(pyroRedPose), al_get_bitmap_height(pyroRedPose),currentResW - 120, 50, 88, 88, 0);
//                            al_draw_scaled_bitmap(pyroStats,0,0, al_get_bitmap_width(pyroStats), al_get_bitmap_height(pyroStats),currentResW - 112, 140, 80, 80, 0);
//                        }
//                        if(!selected)
//                        {
//                            al_draw_bitmap(cursor1,currentResW/2 - al_get_bitmap_width(cursor1)/2 - 35, currentMenuSelection*currentResH/4 + currentResH/4,0);
//                        }
//                        else
//                        {
//                            al_draw_bitmap(cursorB,currentResW/2 - al_get_bitmap_width(cursorB)/2 - 35, currentMenuSelection*currentResH/4 + currentResH/4,0);
//                        }
//                        if(!selected2)
//                        {
//                            al_draw_bitmap(cursor2,currentResW/2 - al_get_bitmap_width(cursor2)/2 + 35, currentMenuSelection2*currentResH/4 + currentResH/4,0);
//                        }
//                        else
//                        {
//                            al_draw_bitmap(cursor2B,currentResW/2 - al_get_bitmap_width(cursor2B)/2 + 35, currentMenuSelection2*currentResH/4 + currentResH/4,0);
//                        }
//                        al_draw_textf(font2, al_map_rgb(255,255,255),currentResW /2,5, ALLEGRO_ALIGN_CENTRE,"PLAYER SELECT");
//                        al_draw_textf(font, al_map_rgb(255,255,255),currentResW /2,currentResH/4, ALLEGRO_ALIGN_CENTRE,"GUNNER");
//                        al_draw_textf(font, al_map_rgb(255,255,255),currentResW /2,currentResH/2, ALLEGRO_ALIGN_CENTRE,"NINJA");
//                        al_draw_textf(font, al_map_rgb(255,255,255),currentResW /2,currentResH*3/4, ALLEGRO_ALIGN_CENTRE,"PYRO");
//                        al_flip_display();
//                    }
//                }
//                else if(menu == MENU_LEVEL_SELECT)
//                {
//                    currentMenuSelection += currentMenuSelection2;
//                    currentMenuSelection2 = 0;
//                    if(currentMenuSelection < 0)
//                    {
//                        currentMenuSelection = maps.size() - 1;
//                    }
//                    else if(currentMenuSelection >= maps.size())
//                    {
//                        currentMenuSelection = 0;
//                    }
//                    if(selected or selected2)
//                    {
//                        doExit = true;
//                        done = true;
//                        currentMapInt = currentMenuSelection;
//                    }
//                    if(exited or exited2)
//                    {
//                        selected = false;
//                        selected2 = false;
//                        exited = false;
//                        exited2 = false;
//                        currentMenuSelection = 0;
//                        currentMenuSelection2 = 0;
//                        menu = MENU_PLAYER_SELECT;
//                    }
//                    if(menu == MENU_LEVEL_SELECT)
//                    {
//                        al_clear_to_color(al_map_rgb(0,0,0));
//                        al_draw_textf(font2, al_map_rgb(255,255,255),currentResW /2,5, ALLEGRO_ALIGN_CENTRE,"LEVEL SELECT");
//                        al_draw_scaled_bitmap(mapPreviews.at(currentMenuSelection),0,0, al_get_bitmap_width(mapPreviews.at(currentMenuSelection)), al_get_bitmap_height(mapPreviews.at(currentMenuSelection)),30, 30, currentResW - 60, currentResH - 60, 0);
//                        al_draw_textf(font, al_map_rgb(255,255,255),currentResW /2,currentResH - 20, ALLEGRO_ALIGN_CENTRE,mapNames.at(currentMenuSelection).data());
//                        al_flip_display();
//                    }
//                }
//            }
//        }
//        if(quit)
//        {
//            al_destroy_event_queue(event_queue);
//            al_destroy_sample(introSong);
//            al_destroy_sample(selectSFX);
//            al_destroy_sample(chooseSFX);
//            al_destroy_sample(cancelSFX);
//            al_destroy_bitmap(cursor1);
//            al_destroy_bitmap(cursor2);
//            al_destroy_sample(gameSong);
//            al_destroy_display(display);
//            al_destroy_timer(timer);
//            al_destroy_bitmap(bouncer);
//            al_destroy_bitmap(bouncer2);
//            al_destroy_bitmap(bulletBitmap);
//            al_destroy_bitmap(mineBitmap);
//            al_destroy_bitmap(blockBitmap);
//            al_destroy_bitmap(someBitmap);
//            al_destroy_font(font);
//            al_destroy_font(font2);
//            al_destroy_sample(versusSong);
//            return 0;
//        }
//        al_stop_sample(&introSongID);
//        al_play_sample(versusSong, 1.7, 0, 1,ALLEGRO_PLAYMODE_ONCE, &introSongID);
//        al_clear_to_color(al_map_rgb(0,0,0));
//        if(characterSelectionPlayer1 == 0)
//        {
//            al_draw_bitmap(gunnerBlueVS,0,0,0);
//        }else if(characterSelectionPlayer1 == 1)
//        {
//            al_draw_bitmap(ninjaBlueVS,0,0,0);
//        }else if(characterSelectionPlayer1 == 2)
//        {
//            al_draw_bitmap(pyroBlueVS,0,0,0);
//        }
//        if(characterSelectionPlayer2 == 0)
//        {
//            al_draw_bitmap(gunnerRedVS,0,0,0);
//        }else if(characterSelectionPlayer2 == 1)
//        {
//            al_draw_bitmap(ninjaRedVS,0,0,0);
//        }else if(characterSelectionPlayer2 == 2)
//        {
//            al_draw_bitmap(pyroRedVS,0,0,0);
//        }
//        al_draw_textf(font2, al_map_rgb(255,255,255),currentResW/2,currentResH/2, ALLEGRO_ALIGN_CENTRE,"VERSUS");
//
//        al_flip_display();
//        std::string tempString = playerSelectionGraphics.at(characterSelectionPlayer1);
//        tempString.append("blue");
//        player p1(1,46,40,p1Controls,playerSelection.at(characterSelectionPlayer1),tempString,iniFile);
//        tempString = playerSelectionGraphics.at(characterSelectionPlayer2);
//        tempString.append("red");
//        player p2(-1,currentResW - 54,40,p2Controls,playerSelection.at(characterSelectionPlayer2),tempString,iniFile);
//        getValue currentMap(maps.at(currentMapInt), "maps", iniFile);
//        map level(currentMap.stringValue,currentResW,BOUNCER_SIZE);
//        p1.setMap(level);
//        p2.setMap(level);
//        animation tempAnim(mineExplosion,-8,-18,20,20,true,1);
//        attackHandler::initHandler(&tempAnim,bulletBitmap,mineBitmap,iniFile,attackSettings);
//        hud::initHud(iniFile,hudSettings,level);
//        al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
////        al_rest(1.5);
//        int countDownCounter = 0;
//        int countDownTime = 30;
//        int countDown = 3;
//        al_set_target_bitmap(al_get_backbuffer(display));
//        al_flush_event_queue(event_queue);
//        al_play_sample(countDownSound, 2.7, 0, 1,ALLEGRO_PLAYMODE_ONCE, NULL);
//        while(1)
//        {
//            ALLEGRO_EVENT ev;
//            al_wait_for_event(event_queue, &ev);
//            if(ev.type==ALLEGRO_EVENT_TIMER)
//            {
//                if(++countDownCounter >= countDownTime)
//                {
//                    countDownCounter = 0;
//                    if(--countDown <= 0)
//                    {
//                        break;
//                    }
//                    al_play_sample(countDownSound, 2.7, 0, 1,ALLEGRO_PLAYMODE_ONCE, NULL);
//                }
//                al_clear_to_color(al_map_rgb(0,0,0));
//                if(characterSelectionPlayer1 == 0)
//                {
//                    al_draw_bitmap(gunnerBlueVS,0,0,0);
//                }else if(characterSelectionPlayer1 == 1)
//                {
//                    al_draw_bitmap(ninjaBlueVS,0,0,0);
//                }else if(characterSelectionPlayer1 == 2)
//                {
//                    al_draw_bitmap(pyroBlueVS,0,0,0);
//                }
//                if(characterSelectionPlayer2 == 0)
//                {
//                    al_draw_bitmap(gunnerRedVS,0,0,0);
//                }else if(characterSelectionPlayer2 == 1)
//                {
//                    al_draw_bitmap(ninjaRedVS,0,0,0);
//                }else if(characterSelectionPlayer2 == 2)
//                {
//                    al_draw_bitmap(pyroRedVS,0,0,0);
//                }
//                al_draw_filled_rectangle(0,0,currentResW,currentResH,al_map_rgba(0,0,0, 80 * (4-countDown)));
//                al_draw_textf(font2, al_map_rgb(255,255,255),currentResW/2,currentResH/2, ALLEGRO_ALIGN_CENTRE,"%i", countDown);
//                al_flip_display();
//            }
//        }
//        al_stop_sample(&introSongID);
//        al_play_sample(gameSong, 1.7, 0, 1,ALLEGRO_PLAYMODE_LOOP, &introSongID);
//        doExit = false;
//        bool paused = false;
//        al_play_sample(goSound, 2.7, 0, 1,ALLEGRO_PLAYMODE_ONCE, NULL);
//        int numDeaths1 = 0;
//        int numDeaths2 = 0;
//        while(!doExit and !quit)
//        {
//            ALLEGRO_EVENT ev;
//            al_wait_for_event(event_queue, &ev);
//            p1.update(ev);
//            p2.update(ev);
//            if(ev.type == ALLEGRO_EVENT_TIMER)
//            {
//                if(countDownCounter < countDownTime)
//                {
//                    ++countDownCounter;
//                }
//                attackHandler::updateAttacks(level);
//                redraw = true;
//                if(p1.numDeaths != numDeaths1 or p2.numDeaths != numDeaths2)
//                {
//                    if(!classicMode)
//                    {
//                        p1.kill();
//                        p2.kill();
//                        numDeaths1 = --p1.numDeaths;
//                        numDeaths2 = --p2.numDeaths;
//                        attackHandler::reset();
//                        attackHandler::reset(p1);
//                        attackHandler::reset(p2);
//                    }else
//                    {
//                        numDeaths1 = p1.numDeaths;
//                        numDeaths2 = p2.numDeaths;
//                    }
//                    al_play_sample(death, 2.7, 0, 1,ALLEGRO_PLAYMODE_ONCE, NULL);
//                    bool done = false;
//                    int tempInt = 0;
//                    int currentFrame = 0;
//                    al_flush_event_queue(event_queue);
//                    while(!done)
//                    {
//                        ALLEGRO_EVENT pEv;
//                        al_wait_for_event(event_queue,&pEv);
//                        if(pEv.type == ALLEGRO_EVENT_TIMER)
//                        {
//                            if(++tempInt >= screenFlashAnim.speed())
//                            {
//                                if(++currentFrame >= screenFlashAnim.frames())
//                                {
//                                    done = true;
//                                    --currentFrame;
//                                }
//                                tempInt = 0;
//                            }
//                            screenFlashAnim.draw(0,0,1,0,0,currentFrame);
//                            al_flip_display();
//                        }
//                    }
//                }
//                if(p1.numDeaths >= 3 or p2.numDeaths >= 3)
//                {
//                    al_stop_sample(&introSongID);
//                    al_play_sample(victorySong, 2.7, 0, 1,ALLEGRO_PLAYMODE_LOOP, &introSongID);
//                    p1.kill();
//                    p2.kill();
//                    numDeaths1 = --p1.numDeaths;
//                    numDeaths2 = --p2.numDeaths;
//                    attackHandler::reset();
//                    attackHandler::reset(p1);
//                    attackHandler::reset(p2);
//                    al_flush_event_queue(event_queue);
//                    paused = true;
//                    al_draw_filled_rectangle(0,0,currentResW,currentResH,al_map_rgba(0,0,0,255));
//                    if(p2.numDeaths >= 3)
//                    {
//                        al_draw_textf(font2, al_map_rgb(255,255,255),currentResW/2,currentResH/2 - 40, ALLEGRO_ALIGN_CENTRE,"Player 1 Wins!");
//                    }
//                    else
//                    {
//                        al_draw_textf(font, al_map_rgb(255,255,255),currentResW/2,currentResH/2 - 40, ALLEGRO_ALIGN_CENTRE,"Player 2 Wins!");
//                    }
//                    al_draw_textf(font, al_map_rgb(255,255,255),currentResW/2,currentResH/2, ALLEGRO_ALIGN_CENTRE,"Press ENTER to exit to Main Menu");
//                    al_draw_textf(font, al_map_rgb(255,255,255),currentResW/2,currentResH/2 + 20, ALLEGRO_ALIGN_CENTRE,"or press ESC to rematch");
//                    al_flip_display();
//                    numDeaths1 = p1.numDeaths = 0;
//                    numDeaths2 = p2.numDeaths = 0;
//                    while(paused)
//                    {
//                        ALLEGRO_EVENT pEv;
//                        al_wait_for_event(event_queue,&pEv);
//                        if(pEv.type == ALLEGRO_EVENT_KEY_DOWN)
//                        {
//                            if(pEv.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
//                            {
//                                al_stop_sample(&introSongID);
//                                paused = false;
//                                al_play_sample(countDownSound, 2.7, 0, 1,ALLEGRO_PLAYMODE_ONCE, NULL);
//                                countDownCounter = 0;
//                                countDown = 3;
//                                while(1)
//                                {
//                                    ALLEGRO_EVENT ev;
//                                    al_wait_for_event(event_queue, &ev);
//                                    if(ev.type==ALLEGRO_EVENT_TIMER)
//                                    {
//                                        if(++countDownCounter >= countDownTime)
//                                        {
//                                            countDownCounter = 0;
//                                            if(--countDown <= 0)
//                                            {
//                                                break;
//                                            }
//                                            al_play_sample(countDownSound, 2.7, 0, 1,ALLEGRO_PLAYMODE_ONCE, NULL);
//                                        }
//                                        al_clear_to_color(al_map_rgba(0,0,0, 130));
//                                        al_draw_textf(font2, al_map_rgb(255,255,255),currentResW/2,currentResH/2, ALLEGRO_ALIGN_CENTRE,"%i", countDown);
//                                        al_flip_display();
//                                    }
//                                }
//                                al_play_sample(goSound, 2.7, 0, 1,ALLEGRO_PLAYMODE_ONCE, NULL);
//                            }
//                            else if(pEv.keyboard.keycode == ALLEGRO_KEY_ENTER)
//                            {
//                                paused = false;
//                                redraw = false;
//                                doExit = true;
//                            }
//                        }
//                        else if(pEv.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
//                        {
//                            paused = false;
//                            redraw = false;
//                            doExit = true;
//                            quit = true;
//                        }
//                    }
//                    al_play_sample(gameSong, 2.7, 0, 1,ALLEGRO_PLAYMODE_LOOP, &introSongID);
//                }
//            }
//            else if(ev.type == ALLEGRO_EVENT_KEY_DOWN)
//            {
//                if(ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
//                {
//                    al_flush_event_queue(event_queue);
//                    paused = true;
//                    al_draw_filled_rectangle(0,0,currentResW,currentResH,al_map_rgba(0,0,0, 130));
//                    al_draw_textf(font, al_map_rgb(255,255,255),currentResW/2,currentResH/2, ALLEGRO_ALIGN_CENTRE,"Press ENTER to exit to Main Menu");
//                    al_draw_textf(font, al_map_rgb(255,255,255),currentResW/2,currentResH/2 + 20, ALLEGRO_ALIGN_CENTRE,"or press ESC to return");
//                    al_flip_display();
//                    while(paused)
//                    {
//                        ALLEGRO_EVENT pEv;
//                        al_wait_for_event(event_queue,&pEv);
//                        if(pEv.type == ALLEGRO_EVENT_KEY_DOWN)
//                        {
//                            if(pEv.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
//                            {
//                                paused = false;
//                            }
//                            else if(pEv.keyboard.keycode == ALLEGRO_KEY_ENTER)
//                            {
//                                paused = false;
//                                redraw = false;
//                                doExit = true;
//                                p1.kill();
//                                p2.kill();
//                                numDeaths1 = --p1.numDeaths;
//                                numDeaths2 = --p2.numDeaths;
//                                attackHandler::reset();
//                                attackHandler::reset(p1);
//                                attackHandler::reset(p2);
//                            }
//                        }
//                        else if(pEv.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
//                        {
//                            paused = false;
//                            redraw = false;
//                            doExit = true;
//                            quit = true;
//                        }
//                    }
//                }
//            }
//            else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
//            {
//                paused = true;
//                al_draw_filled_rectangle(0,0,currentResW,currentResH,al_map_rgba(0,0,0, 130));
//                al_draw_textf(font, al_map_rgb(255,255,255),currentResW/2,currentResH/2, ALLEGRO_ALIGN_CENTRE,"Press ENTER to exit to Main Menu");
//                al_draw_textf(font, al_map_rgb(255,255,255),currentResW/2,currentResH/2 + 20, ALLEGRO_ALIGN_CENTRE,"or press ESC to return");
//                al_flip_display();
//                while(paused)
//                {
//                    ALLEGRO_EVENT pEv;
//                    al_wait_for_event(event_queue,&pEv);
//                    if(pEv.type == ALLEGRO_EVENT_KEY_DOWN)
//                    {
//                        if(pEv.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
//                        {
//                            paused = false;
//                        }
//                        else if(pEv.keyboard.keycode == ALLEGRO_KEY_ENTER)
//                        {
//                            paused = false;
//                            redraw = false;
//                            doExit = true;
//                            p1.kill();
//                            p2.kill();
//                            numDeaths1 = --p1.numDeaths;
//                            numDeaths2 = --p2.numDeaths;
//                            attackHandler::reset();
//                            attackHandler::reset(p1);
//                            attackHandler::reset(p2);
//                        }
//                    }
//                    else if(pEv.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
//                    {
//                        paused = false;
//                        redraw = false;
//                        doExit = true;
//                        quit = true;
//                    }
//                }
//            }
//            if(redraw && al_is_event_queue_empty(event_queue))
//            {
//                redraw = false;
//                al_clear_to_color(al_map_rgb(100,100,99));
//                level.draw();
//                p1.draw();
//                p2.draw();
//                attackHandler::draw();
//                hud::draw(currentResW);
//                al_draw_textf(font2, al_map_rgb(255,255,255),9,14, ALLEGRO_ALIGN_CENTRE,"%i",p2.numDeaths);
//                al_draw_textf(font2, al_map_rgb(255,255,255),currentResW-9,14, ALLEGRO_ALIGN_CENTRE,"%i",p1.numDeaths);
//                if(countDownCounter < countDownTime)
//                {
//                    al_draw_textf(font2, al_map_rgb(255,255,255),currentResW/2,currentResH/2, ALLEGRO_ALIGN_CENTRE,"GO!");
//                }
//                //            animHandler::draw(20,20,1,0);
//                //            al_draw_bitmap(someBitmap,20,20,0);
//                al_flip_display();
//            }
//        }
//        al_stop_sample(&introSongID);
//        menu=MENU_TITLE_SCREEN;
//        doExit = false;
//    }
//    al_destroy_event_queue(event_queue);
//    al_destroy_display(display);
//    al_destroy_timer(timer);
//    al_destroy_sample(introSong);
//    al_destroy_sample(victorySong);
//    al_destroy_sample(selectSFX);
//    al_destroy_sample(chooseSFX);
//    al_destroy_sample(cancelSFX);
//    al_destroy_sample(countDownSound);
//    al_destroy_sample(goSound);
//    al_destroy_bitmap(cursor1);
//    al_destroy_bitmap(cursor2);
//    al_destroy_bitmap(cursorB);
//    al_destroy_bitmap(cursor2B);
//    al_destroy_sample(gameSong);
//    al_destroy_bitmap(bouncer);
//    al_destroy_bitmap(bouncer2);
//    al_destroy_bitmap(bulletBitmap);
//    al_destroy_bitmap(mineBitmap);
//    al_destroy_bitmap(blockBitmap);
//    al_destroy_bitmap(someBitmap);
//    al_destroy_font(font);
//    al_destroy_font(font2);
//    al_destroy_sample(versusSong);
//    al_destroy_bitmap(gunnerBluePose);
//    al_destroy_bitmap(gunnerRedPose);
//    al_destroy_bitmap(ninjaBluePose);
//    al_destroy_bitmap(ninjaRedPose);
//    al_destroy_bitmap(pyroBluePose);
//    al_destroy_bitmap(pyroRedPose);
//    al_destroy_bitmap(gunnerStats);
//    al_destroy_bitmap(ninjaStats);
//    al_destroy_bitmap(pyroStats);
//    al_destroy_bitmap(titleLogo);
//    al_destroy_bitmap(mineExplosion);
//    return 0;
//}
