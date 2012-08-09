#include "animHandler.hpp"
void animHandler::setHandler(animation &animToSet)
{
    myAnimation = &animToSet;
    frameCounter = 0;
    currentFrame = 0;
}
bool animHandler::update()
{
    if(myAnimation != NULL)
    {
        if(++frameCounter > myAnimation->speed())
        {
            frameCounter = 0;
            ++currentFrame;
            if(currentFrame >= myAnimation->frames())
            {
                if(myAnimation->oneThrough())
                {
                    currentFrame = 0;
                    return true;
                }else
                {
                    currentFrame = 0;
                    return false;
                }
            }
        }
        return false;
    }
    return false;
}
void animHandler::draw(float x, float y, bool facingRight)
{

    myAnimation->draw(x,y,facingRight,currentFrame);
}
//void animHandler::draw(float x, float y, int horizontal, int vertical, int facing)
//{
//
//    myAnimation->draw(x,y,horizontal,vertical,facing,currentFrame);
//}
