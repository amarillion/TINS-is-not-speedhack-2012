#include "polygon.hpp"

game::vector game::polygon::edge(int num)
{
    if(num == 0)
    {
        if(points_.size() > 0)
        {
            game::vector tempVector = originPoint_ - points_.at(0);
            return tempVector;
        }else
        {
            fprintf(stderr, "Attempted to get edge from uninitialized polygon\n");
            exit(EXIT_FAILURE);
        }
    }else
    {
        if(num > points_.size())
        {
            fprintf(stderr, "Attempted to get edge beyond polygon edges\n");
            exit(EXIT_FAILURE);
        }
        --num;
        if(points_.size() == 0)
        {
            fprintf(stderr, "Attempted to get edge from uninitialized polygon\n");
            exit(EXIT_FAILURE);
        }
        game::vector tempVector = points_.at(num + 1) - points_.at(num);
        return tempVector;
    }
}
game::vector game::polygon::point(int num)
{
    if(num == 0)
    {
        if(points_.size() > 0)
        {
            return originPoint_;
        }else
        {
            fprintf(stderr, "Attempted to get point from uninitialized polygon\n");
            exit(EXIT_FAILURE);
        }
    }else
    {
        if(num > points_.size())
        {
            fprintf(stderr, "Attempted to get point beyond polygon points\n");
            exit(EXIT_FAILURE);
        }
        --num;
        if(points_.size() == 0)
        {
            fprintf(stderr, "Attempted to get point from uninitialized polygon\n");
            exit(EXIT_FAILURE);
        }
        return points_.at(num);
    }
}
game::vector game::polygon::getCenter()
{
    float tempX = 0;
    float tempY = 0;
    tempX += originPoint_.getX()*(1 + points_.size());
    tempY += originPoint_.getY()*(1 + points_.size());
    for(int i = 0; i < points_.size(); ++i)
    {
        tempX += points_.at(i).getX();
        tempY += points_.at(i).getY();
    }
    game::vector tempVector(tempX,tempY,false);
    return tempVector;
}
