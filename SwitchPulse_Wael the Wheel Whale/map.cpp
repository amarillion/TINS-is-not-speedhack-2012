#include "map.hpp"
mapLevel::mapLevel(std::string iniFile, std::string section)
{
    getValue delimitedText,blockDimensions,width,blockSheet,blockType,bgR,bgG,bgB;
    delimitedText.update("blocks",section,iniFile);
    blockDimensions.update("width",section,iniFile);
    width.update("blockWidth",section,iniFile);
    blockSheet.update("blockSheet",section,iniFile);
    bgR.update("bgRed",section,iniFile);
    bgG.update("bgGreen",section,iniFile);
    bgB.update("bgBlue",section,iniFile);
    bgColor = new ALLEGRO_COLOR();
    bgColor->r =(int)bgR.numValue;
    bgColor->g =(int)bgG.numValue;
    bgColor->b =(int)bgB.numValue;
    sheet =  al_load_bitmap(blockSheet.stringValue.c_str());
    al_convert_mask_to_alpha(sheet,al_map_rgb(0,0,0));
    width_ = width.numValue;
    Splitter tokens(delimitedText.stringValue,",");
    for(int i = 0; i < tokens.size(); ++i)
    {
        if(atoi(tokens[i].data()) != 0)
        {
            int tempX = (i % (int)blockDimensions.numValue)*width_;
            int tempY = (i / (int)blockDimensions.numValue)*width_;
            block temp(tempX,tempY,width_,width_);
            blockType.update("block" + tokens[i],section,iniFile);
            temp.type = blockType.numValue;
            temp.frame = atoi(tokens[i].data());
            blocks_.push_back(temp);
        }
    }
    std::string section1 = section + "bg1";
    std::string section2 = section + "bg2";
    bg1 = new mapLevel(iniFile,section1,0);
    bg2 = new mapLevel(iniFile,section2,0);
}
mapLevel::mapLevel(std::string iniFile, std::string section, int bgFlag)
{
    getValue delimitedText,blockDimensions,width,blockSheet,blockType;
    delimitedText.update("blocks",section,iniFile);
    blockDimensions.update("width",section,iniFile);
    width.update("blockWidth",section,iniFile);
    blockSheet.update("blockSheet",section,iniFile);
    sheet =  al_load_bitmap(blockSheet.stringValue.c_str());
    al_convert_mask_to_alpha(sheet,al_map_rgb(0,0,0));
    width_ = width.numValue;
    Splitter tokens(delimitedText.stringValue,",");
    for(int i = 0; i < tokens.size(); ++i)
    {
        if(atoi(tokens[i].data()) != 0)
        {
            int tempX = (i % (int)blockDimensions.numValue)*width_;
            int tempY = (i / (int)blockDimensions.numValue)*width_;
            block temp(tempX,tempY,width_,width_);
            blockType.update("block" + tokens[i],section,iniFile);
            temp.type = blockType.numValue;
            temp.frame = atoi(tokens[i].data());
            blocks_.push_back(temp);
        }
    }
    bg1 = NULL;
    bg2 = NULL;
}

void mapLevel::scroll(float xScroll, float yScroll)
{
    for(int i = 0; i < blocks_.size(); i++)
    {
        blocks_.at(i).setX(blocks_.at(i).getX() + xScroll);
        blocks_.at(i).setY(blocks_.at(i).getY() + yScroll);
    }
    if(bg1 != NULL)
        bg1->scroll(xScroll*.5,yScroll*.5);
    if(bg2 != NULL)
        bg2->scroll(xScroll*.25,yScroll*.25);
}
mapLevel::~mapLevel()
{
    al_destroy_bitmap(sheet);
    if(bg1 != NULL)
        delete(bg1);
    if(bg2 != NULL)
        delete(bg2);
}
void mapLevel::draw()
{
    al_clear_to_color(*bgColor);
    if(bg2!=NULL)
        bg2->drawLoop();
    if(bg1 != NULL)
        bg1->drawLoop();
    for(int i = 0; i < blocks_.size(); i++)
    {
        al_draw_bitmap_region(sheet, (float)((blocks_.at(i).frame-1)*width_), 0, (float)width_, (float)width_, blocks_.at(i).getX(), blocks_.at(i).getY(), 0);
    }
}
void mapLevel::drawLoop()
{
    for(int i = 0; i < blocks_.size(); i++)
    {
        if(blocks_.at(i).getX() < 0)
        {
            al_draw_bitmap_region(sheet, (float)((blocks_.at(i).frame-1)*width_), 0, (float)width_, (float)width_, blocks_.at(i).getX()+320, blocks_.at(i).getY(), 0);
            if(blocks_.at(i).getX() + blocks_.at(i).getWidth() < 0)
            {
                blocks_.at(i).setX(blocks_.at(i).getX()+320);
            }
        }
        if(blocks_.at(i).getX() + blocks_.at(i).getWidth() > 320)
        {
            al_draw_bitmap_region(sheet, (float)((blocks_.at(i).frame-1)*width_), 0, (float)width_, (float)width_, blocks_.at(i).getX()-320, blocks_.at(i).getY(), 0);
            if(blocks_.at(i).getX() > 320)
            {
                blocks_.at(i).setX(blocks_.at(i).getX()-320);
            }
        }
        al_draw_bitmap_region(sheet, (float)((blocks_.at(i).frame-1)*width_), 0, (float)width_, (float)width_, blocks_.at(i).getX(), blocks_.at(i).getY(), 0);
    }
}
//float map::collide(player & activePlayer, bool movingHorizontally, bool positive)
//{
//    player * tempPointer = &activePlayer;
//    bool tempBool = false;
//    int tempInt = 0;
//    if(registeredPlayers_.size() != 0)
//    {
//        for(int i = 0; i < registeredPlayers_.size(); ++i)
//        {
//            if(tempPointer == registeredPlayers_.at(i))
//            {width_
//                playerBlocks_.at(i) = activePlayer.getBoundingBox();
//                tempInt = i;
//                tempBool = true;
//            }
//        }
//    }
//    if(!tempBool)
//    {
//        fprintf(stderr, "Unregistered Player was requested to update");
//        exit(EXIT_FAILURE);
//    }
//
//    for(int i = 0; i < playerBlocks_.size() ;++i)
//    {
//        if(i != tempInt)// and !playerBlocks_.at(i)->isBumped)
//        {
//            if(playerBlocks_.at(i).collide(activePlayer.getBoundingBox()))
//            {
//                al_play_sample(bump_,1,0,1,ALLEGRO_PLAYMODE_ONCE,NULL);
//                if(movingHorizontally)
//                {
//                    if(positive)
//                    {
//                        float tempNum = playerBlocks_.at(i).getX() - activePlayer.getWidth();
//                        registeredPlayers_.at(i)->bump(true,true);
//                        activePlayer.bump(true,false);
//                        return tempNum;
//                    }else
//                    {
//                        float tempNum = playerBlocks_.at(i).getX() + playerBlocks_.at(i).getWidth();
//                        registeredPlayers_.at(i)->bump(true,false);
//                        activePlayer.bump(true,true);
//                        return tempNum;
//                    }
//                }else
//                {
//
//                    if(activePlayer.getX() <= registeredPlayers_.at(i)->getX())
//                    {
//                        registeredPlayers_.at(i)->bump(true,true);
//                        activePlayer.bump(true,false);
//                    }else
//                    {
//                        registeredPlayers_.at(i)->bump(true,false);
//                        activePlayer.bump(true,true);
//                    }
//                    if(positive)
//                    {
//                        float tempNum = playerBlocks_.at(i).getY() - activePlayer.getHeight();
//                        registeredPlayers_.at(i)->bump(false,true);
//                        activePlayer.bump(false,false);
//                        return tempNum;
//                    }else
//                    {
//                        float tempNum = playerBlocks_.at(i).getY() + playerBlocks_.at(i).getHeight();
//                        registeredPlayers_.at(i)->bump(false,false);
//                        activePlayer.bump(false,true);
//                        return tempNum;
//                    }
//                }
//            }
//        }
//    }
//    for(int i = 0; i < blocks_.size();++i)
//    {
//        if(blocks_.at(i).collide(activePlayer.getBoundingBox()))
//        {
//            if(movingHorizontally)
//            {
//                if(positive)
//                {
//                    float tempNum = blocks_.at(i).getX() - activePlayer.getWidth();
//                    return tempNum;
//                }else
//                {
//                    float tempNum = blocks_.at(i).getX() + blocks_.at(i).getWidth();
//                    return tempNum;
//                }
//            }else
//            {
//                if(positive)
//                {
//                    float tempNum = blocks_.at(i).getY() - activePlayer.getHeight();
//                    return tempNum;
//                }else
//                {
//                    float tempNum = blocks_.at(i).getY() + blocks_.at(i).getHeight();
//                    return tempNum;
//                }
//            }
//        }
//    }
//    if(movingHorizontally)
//    {
//        return activePlayer.getX();
//    }else
//    {
//        return activePlayer.getY();
//    }
//}
//bool map::collide(block bullet)
//{
//    for(int i = 0; i < blocks_.size();++i)
//    {
//        if(blocks_.at(i).collide(bullet))
//        {
//            return true;
//        }
//    }
//    return false;
//}
