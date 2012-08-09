#ifndef life5_block_hpp
#define life5_block_hpp
#include <allegro5/allegro.h>
class block
{
    public:
    block(){};
    block(float x, float y, float width, float height) : x_(x),y_(y),height_(height),width_(width) {}
    block(float x, float y, float width, float height,ALLEGRO_BITMAP * bitmap) : x_(x),y_(y),height_(height),width_(width),sheet_(bitmap) {}
    void draw();
    int getWidth() {return width_;}
    int getHeight() {return height_;}
    //void hit() {};
    float getX() {return x_;}
    float getY() {return y_;}
    void setX(float newX) {x_ = newX;}
    void setY(float newY) {y_ = newY;}
    int collide(block other);
    int extendCollide(block other, float extend);
    int type;
    int frame;
    //private:
    ALLEGRO_BITMAP * sheet_;
    float x_,y_;
    float height_,width_;
};
#endif

