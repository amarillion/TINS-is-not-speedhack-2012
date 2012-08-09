#ifndef life5_map_hpp
#define life5_map_hpp
#include <allegro5/allegro.h>
#include <vector>
#include <iostream>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include "block.hpp"
#include "splitter.cpp"
#include "getValue.hpp"

class mapLevel;
class mapLevel
{
    public:
    mapLevel(std::string iniFile, std::string section);
    mapLevel(std::string iniFile, std::string section, int bgFlag);
    void scroll(float xScroll, float yScroll);
    std::vector<block> getBlocks(){return blocks_;}
    ~mapLevel();
    void draw();
    void drawLoop();
    private:
    std::vector<block> blocks_;
    mapLevel *bg1;
    mapLevel *bg2;
    ALLEGRO_BITMAP * sheet;
    ALLEGRO_COLOR *bgColor;
    int width_;
};
#endif
