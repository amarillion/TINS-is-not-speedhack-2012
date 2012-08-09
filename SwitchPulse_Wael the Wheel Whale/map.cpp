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
