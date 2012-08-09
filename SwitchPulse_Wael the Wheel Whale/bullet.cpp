#include "bullet.hpp"
void bullet::setMove(float x, float y)
{
    xMove_ = x;
    yMove_ = y;
}
void bullet::update()
{
    x_ += xMove_;
    y_ += yMove_;
}
