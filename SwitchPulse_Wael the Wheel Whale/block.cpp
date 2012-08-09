#include "block.hpp"
#include <stdio.h>
int block::collide(block other)
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
int block::extendCollide(block other, float extend)
{
    if(x_ >= other.x_ + other.width_ + extend)
    {
        return 0;
    }else if(x_ + width_ + extend<= other.x_)
    {
        return 0;
    }else if(y_ >= other.y_ + other.height_ + extend)
    {
        return 0;
    }else if(y_ + height_ + extend <= other.y_)
    {
        return 0;
    }else
    {
        return 1;
    }
}
void block::draw()
{
    al_draw_bitmap(sheet_, x_, y_, 0);
}
