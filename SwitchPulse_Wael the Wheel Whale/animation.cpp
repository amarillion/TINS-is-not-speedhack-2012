#include "animation.hpp"
animation::animation(ALLEGRO_BITMAP * spriteSheet,int xOffset,int yOffset,int width, int height,bool oneThrough,int speedTemp)
        :speed_(speedTemp),xOffset_(xOffset),yOffset_(yOffset),oneThrough_(oneThrough),split_(false)
{
    int bitmapWidth = al_get_bitmap_width(spriteSheet);
    int bitmapHeight = al_get_bitmap_height(spriteSheet);
    if(bitmapHeight % height != 0 or bitmapWidth % width != 0)
    {
        //Malformed settings.ini or malformed sprite sheet
        fprintf(stderr, "NIGGER? %i %i\n",width,height);
    }else
    {
        for(int i = 0; i < bitmapWidth / width; ++i)
        {
            for(int j = 0; j < bitmapHeight / height; ++j)
            {
                ALLEGRO_BITMAP * tempBitmap = al_create_bitmap(width,height);
                al_set_target_bitmap(tempBitmap);
                al_clear_to_color(al_map_rgb(100,100,99));
                al_draw_bitmap_region(spriteSheet,i*width, j*height, width, height, 0, 0, 0);
                al_convert_mask_to_alpha(tempBitmap,al_map_rgb(0,0,0));
                frames_.push_back(tempBitmap);
            }
        }
    }
}
animation::animation(ALLEGRO_BITMAP * spriteSheet,ALLEGRO_BITMAP * bottomSpriteSheet, int xOffset,int yOffset,int width, int height, int xBottomOffset, int yBottomOffset,int bottomWidth, int bottomHeight,int speedTemp)
        :speed_(speedTemp),xOffset_(xOffset),yOffset_(yOffset),oneThrough_(false),xBottomOffset_(xBottomOffset),yBottomOffset_(yBottomOffset),split_(true)
{
    int bitmapWidth = al_get_bitmap_width(spriteSheet);
    int bitmapHeight = al_get_bitmap_height(spriteSheet);
    if(bitmapHeight % height != 0 or bitmapWidth % width != 0)
    {
        //Malformed settings.ini or malformed sprite sheet
        fprintf(stderr, "NIGGERFAGGOT?\n");
    }else
    {
        for(int i = 0; i < bitmapWidth / width; ++i)
        {
            for(int j = 0; j < bitmapHeight / height; ++j)
            {
                ALLEGRO_BITMAP * tempBitmap = al_create_bitmap(width,height);
                al_set_target_bitmap(tempBitmap);
                al_clear_to_color(al_map_rgba(100,100,99,0));
                al_draw_bitmap_region(spriteSheet,i*width, j*height, width, height, 0, 0, 0);
                frames_.push_back(tempBitmap);
            }
        }
    }
    int bottomSheetWidth = al_get_bitmap_width(bottomSpriteSheet);
    int bottomSheetHeight = al_get_bitmap_height(bottomSpriteSheet);
    if(bottomSheetHeight % bottomHeight != 0 or bottomSheetWidth % bottomWidth != 0)
    {
        //Malformed settings.ini or malformed sprite sheet
        fprintf(stderr, "tehfuckrlysdf? %i\n", bottomSheetWidth);
        exit(1);
    }else if(bottomSpriteSheet != NULL)
    {
        for(int j = 0; j < bottomSheetHeight / bottomHeight; ++j)
        {
            for(int i = 0; i < bottomSheetWidth / bottomWidth; ++i)
            {
                ALLEGRO_BITMAP * tempBitmap = al_create_bitmap(bottomWidth,bottomHeight);
                al_set_target_bitmap(tempBitmap);
                al_clear_to_color(al_map_rgba(100,100,99,0));
                al_draw_bitmap_region(bottomSpriteSheet,i*bottomWidth, j*bottomHeight, bottomWidth, bottomHeight, 0, 0, 0);
                bottomFrames_.push_back(tempBitmap);
            }
        }
    }
}
animation::~animation()
{
    if(frames_.size() > 0)
    {
        for(int i = 0; i < frames_.size(); ++i)
        {
            al_destroy_bitmap(frames_.at(i));
        }
    }
    if(bottomFrames_.size() > 0)
    {
        for(int i = 0; i < bottomFrames_.size(); ++i)
        {
            al_destroy_bitmap(bottomFrames_.at(i));
        }
    }
}
animation & animation::operator=(const animation &rightValue)
{
    frames_.clear();
    bottomFrames_.clear();
    for(int i = 0; i < rightValue.frames_.size(); ++i)
    {
        ALLEGRO_BITMAP * tempBitmap = al_clone_bitmap(rightValue.frames_.at(i));
        frames_.push_back(tempBitmap);
    }
    for(int i = 0; i < rightValue.bottomFrames_.size(); ++i)
    {
        ALLEGRO_BITMAP * tempBitmap = al_clone_bitmap(rightValue.bottomFrames_.at(i));
        bottomFrames_.push_back(tempBitmap);
    }
    xOffset_ = rightValue.xOffset_;
    yOffset_ = rightValue.yOffset_;
    split_ = rightValue.split_;
    oneThrough_ = rightValue.oneThrough_;
    xBottomOffset_ = rightValue.xBottomOffset_;
    yBottomOffset_ = rightValue.yBottomOffset_;
    speed_ = rightValue.speed_;
    return *this;
}
animation::animation( const animation& other )
{
    *this=other;
}
void animation::draw(float x, float y, bool facingRight, int frame)
{
    if(facingRight)
    {
        al_draw_bitmap(frames_.at(frame), x + xOffset_, y + yOffset_,0);
    }
    else
    {
        al_draw_bitmap(frames_.at(frame), x + xOffset_, y + yOffset_,ALLEGRO_FLIP_HORIZONTAL);
    }
}
//void animation::draw(float x, float y, int horizontal, int vertical, int facing, int frame)
//{
//    if(split_)
//    {
//        if(horizontal > 0)
//        {
//            if(vertical < 0)
//            {
//                al_draw_bitmap(bottomFrames_.at(frame), x + xBottomOffset_, y + yBottomOffset_,0);
//                al_draw_bitmap(frames_.at(DIRECTION_DIAGONAL_UP), x + xOffset_, y + yOffset_, 0);
//            }else if(vertical == 0)
//            {
//                al_draw_bitmap(bottomFrames_.at(frame), x + xBottomOffset_, y + yBottomOffset_,0);
//                al_draw_bitmap(frames_.at(DIRECTION_FORWARD), x + xOffset_, y + yOffset_, 0);
//            }else
//            {
//                al_draw_bitmap(bottomFrames_.at(frame), x + xBottomOffset_, y + yBottomOffset_,0);
//                al_draw_bitmap(frames_.at(DIRECTION_DIAGONAL_DOWN), x + xOffset_, y + yOffset_, 0);
//            }
//        }else if(horizontal == 0)
//        {
//            if(vertical > 0)
//            {
//                if(facing > 0)
//                {
//                    al_draw_bitmap(bottomFrames_.at(frame), x + xBottomOffset_, y + yBottomOffset_,0);
//                    al_draw_bitmap(frames_.at(DIRECTION_DOWN), x + xOffset_, y + yOffset_, 0);
//                }else
//                {
//                    al_draw_bitmap(bottomFrames_.at(frame), x + xBottomOffset_, y + yBottomOffset_,ALLEGRO_FLIP_HORIZONTAL);
//                    al_draw_bitmap(frames_.at(DIRECTION_DOWN), x + xOffset_, y + yOffset_, ALLEGRO_FLIP_HORIZONTAL);
//                }
//            }else if(vertical == 0)
//            {
//                //FUCKING ERRORRRRR HORRY SHITTTTT!!!1!!
//                fprintf(stderr, "tehfuckrly!!?\n");
//            }else
//            {
//                if(facing > 0)
//                {
//                    al_draw_bitmap(bottomFrames_.at(frame), x + xBottomOffset_, y + yBottomOffset_,0);
//                    al_draw_bitmap(frames_.at(DIRECTION_UP), x + xOffset_, y + yOffset_, 0);
//                }else
//                {
//                    al_draw_bitmap(bottomFrames_.at(frame), x + xBottomOffset_, y + yBottomOffset_,ALLEGRO_FLIP_HORIZONTAL);
//                    al_draw_bitmap(frames_.at(DIRECTION_UP), x + xOffset_, y + yOffset_, ALLEGRO_FLIP_HORIZONTAL);
//                }
//            }
//        }else
//        {
//            if(vertical < 0)
//            {
//                al_draw_bitmap(bottomFrames_.at(frame), x + xBottomOffset_, y + yBottomOffset_,ALLEGRO_FLIP_HORIZONTAL);
//                al_draw_bitmap(frames_.at(DIRECTION_DIAGONAL_UP), x + xOffset_, y + yOffset_, ALLEGRO_FLIP_HORIZONTAL);
//            }else if(vertical == 0)
//            {
//                al_draw_bitmap(bottomFrames_.at(frame), x + xBottomOffset_, y + yBottomOffset_,ALLEGRO_FLIP_HORIZONTAL);
//                al_draw_bitmap(frames_.at(DIRECTION_FORWARD), x + xOffset_, y + yOffset_, ALLEGRO_FLIP_HORIZONTAL);
//            }else
//            {
//                al_draw_bitmap(bottomFrames_.at(frame), x + xBottomOffset_, y + yBottomOffset_,ALLEGRO_FLIP_HORIZONTAL);
//                al_draw_bitmap(frames_.at(DIRECTION_DIAGONAL_DOWN), x + xOffset_, y + yOffset_, ALLEGRO_FLIP_HORIZONTAL);
//            }
//        }
//    }else
//    {
//        if(horizontal >= 0)
//        {
//            al_draw_bitmap(frames_.at(frame), x + xOffset_, y + yOffset_, 0);
//        }else
//        {
//            al_draw_bitmap(frames_.at(frame), x + xOffset_, y + yOffset_, ALLEGRO_FLIP_HORIZONTAL);
//        }
//    }
//}
