#ifndef life5_enemy_hpp
#define life5_enemy_hpp
#include "block.hpp"
#include "bullet.hpp"
class enemy: public block
{
    public:
    enemy(float x, float y, float width, float height, ALLEGRO_BITMAP * bitmap,int fireDelay):block(x,y,width,height,bitmap), fDelay(fireDelay), counter(0) {}
    void scroll(float xScroll, float yScroll){x_ += xScroll; y_ += yScroll;}
    bool update();
    int collide(block other)
    {
        if(x_ >= other.x_ + other.width_)
        {
            return 0;
        }else if(x_ + width_ <= other.x_)
        {
            return 0;
        }else if(y_ >= other.y_ + other.height_)
        {
            return 0;
        }else if(y_ + height_ <= other.y_)
        {
            return 0;
        }else
        {
            return 1;
        }
    }
    int collide(bullet other)
    {
        if(x_ >= other.x_ + other.width_)
        {
            return 0;
        }else if(x_ + width_ <= other.x_)
        {
            return 0;
        }else if(y_ >= other.y_ + other.height_)
        {
            return 0;
        }else if(y_ + height_ <= other.y_)
        {
            return 0;
        }else
        {
            return 1;
        }
    }
    private:
    int fDelay;
    int counter;
};
#endif
