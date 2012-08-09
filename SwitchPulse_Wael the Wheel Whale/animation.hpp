#ifndef life5_animation_hpp
#define life5_animation_hpp
#include <allegro5/allegro.h>
#include <vector>
#include <stdio.h>
class animation
{
    public:
    animation(ALLEGRO_BITMAP * spriteSheet,int xOffset,int yOffset,int width, int height,bool oneThrough,int speedTemp);
    animation(ALLEGRO_BITMAP * spriteSheet,ALLEGRO_BITMAP * bottomSpriteSheet, int xOffset,int yOffset,int width, int height, int xBottomOffset, int yBottomOffset,int bottomWidth, int bottomHeight,int speedTemp);
    animation() {};
    animation(const animation& other);
    ~animation();
    void draw(float x, float y, bool facingRight, int frame);
    void draw(float x, float y, int horizontal, int vertical, int facing, int frame);
    animation & operator=(const animation &rightValue);
    bool oneThrough() {return oneThrough_;}
    bool split() {return split_;}
    int speed() {return speed_;}
    int getXOffset() {return xOffset_;}
    int getYOffset() {return yOffset_;}
    int getWidth() {return al_get_bitmap_width(frames_.at(0));}
    int getHeight() {return al_get_bitmap_height(frames_.at(0));}
    int frames()
    {
        if(!split_)
        {
            return frames_.size();
        }
        else
        {
            return bottomFrames_.size();
        }
    }
//    int topFrames() {return frames_.size();}
//    int bottomFrames() {return bottomFrames_.size();}

    private:
    int speed_;
    int xOffset_,yOffset_;
    bool split_, oneThrough_;
    int xBottomOffset_, yBottomOffset_;
    std::vector<ALLEGRO_BITMAP *> frames_, bottomFrames_;
};
#endif
