#ifndef life5_animHandler_hpp
#define life5_animHandler_hpp
#include <allegro5/allegro.h>
#include "animation.hpp"
class animHandler
{
    public:
    animHandler() :myAnimation(NULL),frameCounter(0),currentFrame(0){}
    void setHandler(animation &animToSet);
    bool update();
    void draw(float x, float y, bool facingRight);
    void draw(float x, float y, int horizontal, int vertical, int facing);
    animation *myAnimation;
    int frameCounter,currentFrame;
};
#endif
