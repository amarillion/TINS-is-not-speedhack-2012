#include "vector.hpp"
game::vector::vector(float value1,float value2)
{
    xValue_ = value1;
    yValue_ = value2;
}

float game::vector::getMagnitude()
{
    float magnitude;
    float underTheRadical = xValue*xValue + yValue*yValue;
#ifdef FAST_INVERSE_SQRT_MAGNITUDE
    const float xhalf = 0.5f*underTheRadical;
    union
    {
        float x;
        int i;
    } u;                                 // as far as i can tell, this union is just for the bit level hack that this algorithm uses.
    u.x = underTheRadical;
    u.i = 0x5f3759df - (u.i >> 1);       // evil shit, bro.
    magnitude =  1.0/(u.x*(1.5f - xhalf*u.x*u.x));   // newton step, repeating increases accuracy
#else
#ifdef BABYLONIAN_SQRT_MAGNITUDE
    union
    {
        int i;
        float x;
    } u;
    u.x = underTheRadical;              //uh, hell if i know how THIS shit works.
    u.i = (1<<29) + (u.i >> 1) - (1<<22);
    u.x = u.x + x/u.x;
    magnitude = 0.25f*u.x + x/u.x;
#else
    magnitude = sqrt(underTheRadical);
#endif
    return magnitude;
}

float game::vector::getAngle()
{
    float angle = atan2(yValue_,xValue_);
    return angle;
}

void game::vector::set(float tempFloatX,float tempFloatY)
{
    if(xValue_ != tempFloatX or yValue_ != tempFloatY)
    {
        xValue_ = tempFloatX;
        yValue_ = tempFloatY;
    }
}

vector getNormalVector(vector& vector1)
{
    float underTheRadical = xValue_*xValue_ + yValue_*yValue_;
#ifdef FAST_INVERSE_SQRT_NORMALIZE
    const float xhalf = 0.5f*underTheRadical;
    union
    {
        float x;
        int i;
    } u;                                 // as far as i can tell, this union is just for the bit level hack that this algorithm uses.
    u.x = underTheRadical;
    u.i = 0x5f3759df - (u.i >> 1);       // evil shit, bro.
    float inverseMagnitude =  u.x*(1.5f - xhalf*u.x*u.x);   // newton step, repeating increases accuracy
    vector normalVector(xValue*inverseMagnitude,yValue*inverseMagnitude);
#else
#ifdef BABYLONIAN_SQRT_NORMALIZE
    union
    {
        int i;
        float x;
    } u;
    u.x = underTheRadical;              //uh, hell if i know how THIS shit works.
    u.i = (1<<29) + (u.i >> 1) - (1<<22);
    u.x =       u.x + x/u.x;
    magnitude = 0.25f*u.x + x/u.x;
    vector normalVector(xValue/magnitude,yValue/magnitude);
#else
    magnitude = sqrt(underTheRadical);
    vector normalVector(xValue/magnitude,yValue/magnitude);
#endif
    return normalVector;
}

