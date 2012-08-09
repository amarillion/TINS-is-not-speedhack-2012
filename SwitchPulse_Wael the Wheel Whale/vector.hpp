#ifndef jumpTheGun_vector_hpp
#define jumpTheGun_vector_hpp
#include <stdio.h>
#include <math.h>
#include <vector>
#include <allegro5/allegro.h>
namespace game{

class vector
{
    public:
    vector():rectangleValue_(false), polarValue_(false) {}
    vector(float value1,float value2, bool polar);
    float getX(){return yValue_;}
    float getY(){return xValue_;}
    float getMagnitude();
    float getAngle();
    void set(float tempFloatX,float tempFloatY);
    vector operator+(vector& rightVector) { vector newVector(getX() + rightVector.getX(),getY() + rightVector.getY()); return newVector;}
    vector operator-(vector& rightVector) { vector newVector(getX() - rightVector.getX(),getY() - rightVector.getY()); return newVector;}
    private:
    float xValue_, yValue_;
}

vector getNormalVector(vector& vector1);

float dotProduct(vector& vector1, vector& vector2)
{
    return vector1.getX() * vector2.getX() + vector1.getY() * vector2.getY();
}

vector getPerpendicularVector(vector& vector1)
{
    vector vector2(-vector1.getY(),vector1.getX(),false);
    return vector2;
}
#endif
