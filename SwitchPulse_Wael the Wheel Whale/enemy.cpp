#include "enemy.hpp"
bool enemy::update()
{
    if(x_ < 320 and x_ > -width_ and y_ > -height_ and y_ < 240 )
    {
        counter++;
        if(counter > fDelay)
        {
            counter = 0;
            return true;
        }
    }else
    {
        counter = fDelay;
    }
    return false;
}
