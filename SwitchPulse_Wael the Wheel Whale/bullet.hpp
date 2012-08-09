#ifndef life5_bullet_hpp
#define life5_bullet_hpp
#include "block.hpp"
class bullet: public block
{
    public:
    bullet(float x, float y, float width, float height):block(x,y,width,height) {}
    bullet(float x, float y, float width, float height, ALLEGRO_BITMAP * bitmap):block(x,y,width,height,bitmap) {}
    void setMove(float x, float y);
    void scroll(float xScroll, float yScroll){x_ += xScroll; y_ += yScroll;}
    void update();
    int playerNumber;
    private:
    float xMove_, yMove_;
};
#endif
