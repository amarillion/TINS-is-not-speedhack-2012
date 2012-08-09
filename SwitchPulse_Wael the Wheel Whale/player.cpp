#include "player.hpp"
bool player::update(ALLEGRO_EVENT ev, std::vector<block> map)
{
    if(ev.type == ALLEGRO_EVENT_KEY_DOWN)
    {
        if(ev.keyboard.keycode == ALLEGRO_KEY_A)
        {
            left = true;
        }
        if(ev.keyboard.keycode == ALLEGRO_KEY_D)
        {
            right = true;
        }
        if(ev.keyboard.keycode == ALLEGRO_KEY_J)
        {
            jump = true;
        }
    }
    if(ev.type == ALLEGRO_EVENT_KEY_UP)
    {
        if(ev.keyboard.keycode == ALLEGRO_KEY_A)
        {
            left = false;
        }
        if(ev.keyboard.keycode == ALLEGRO_KEY_D)
        {
            right = false;
        }
        if(ev.keyboard.keycode == ALLEGRO_KEY_J)
        {
            jump = false;
        }
    }
    if(ev.type == ALLEGRO_EVENT_TIMER)
    {
        shooting--;
        if(shooting < 0)
        {
            shooting == 0;
        }
        if(left)
        {
            hitBox.setX(hitBox.getX() - 3.0);
            facingRight = false;
            myHandler.update();
        }else if(right)
        {
            hitBox.setX(hitBox.getX() + 3.0);
            facingRight = true;
            myHandler.update();
        }
        for(int i = 0; i < map.size(); i++)
        {
            if(map.at(i).collide(hitBox) and map.at(i).type == 2)
            {
                return true;
            }
            if(map.at(i).collide(hitBox) and map.at(i).type == 1)
            {
                if(map.at(i).getX() < hitBox.getX())
                {
                    hitBox.setX(map.at(i).getX()+map.at(i).getWidth());
                }
                else
                {
                    hitBox.setX(map.at(i).getX()-hitBox.getWidth());
                }
            }
        }
        if(jump and floor)
        {
            air = -4;
            jump = false;
            currentAnim = 1;
            myHandler.setHandler(anims.at(1));
        }
        air+=.1;
        //std::cout << air << std::endl;
        hitBox.setY(hitBox.getY() + air);
        floor = false;
        bool hit = false;
        for(int i = 0; i < map.size(); i++)
        {
            if(map.at(i).collide(hitBox) and map.at(i).type == 2)
            {
                return true;
            }
            if(map.at(i).collide(hitBox) and map.at(i).type == 1)
            {
                air = 0;
                if(map.at(i).getY() < hitBox.getY())
                {
                    hitBox.setY(map.at(i).getY()+map.at(i).getHeight());
                }
                else
                {
                    hitBox.setY(map.at(i).getY()-hitBox.getHeight());
                    floor = true;
                    if(currentAnim != 0)
                    {
                        currentAnim = 0;
                        myHandler.setHandler(anims.at(0));
                    }
                }
                hit = true;
            }
        }
        if(!hit)
        {
            currentAnim = 1;
            myHandler.setHandler(anims.at(1));
            floor = false;
        }
    }
    return false;
}
void player::scroll(float xScroll, float yScroll)
{
    hitBox.setX(hitBox.getX() + xScroll);
    hitBox.setY(hitBox.getY() + yScroll);
}
void player::draw()
{
    float xOffset = 21;
    float yOffset = -1;
    int flip = 0;
    if(!facingRight)
    {
        xOffset = -5;
        flip = ALLEGRO_FLIP_HORIZONTAL;
    }
    if(currentAnim == 1)
    {
        xOffset = 18;
        if(!facingRight)
        {
            xOffset = -11;
        }
        yOffset = 2;
    }
    //if(floor)
    {
        if(shooting > 0)
        al_draw_bitmap_region(heads,32, 0, 16, 16, hitBox.getX() + xOffset, hitBox.getY() + yOffset, flip);
        else
        al_draw_bitmap_region(heads,0, 0, 16, 16, hitBox.getX() + xOffset, hitBox.getY() + yOffset, flip);
    }
    myHandler.draw(hitBox.getX(), hitBox.getY(), facingRight);
}
