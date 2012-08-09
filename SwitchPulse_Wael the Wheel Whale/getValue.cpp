#include "getValue.hpp"
void saveValue(float tempFloat, std::string name, std::string section, std::string iniFile)
{
    ALLEGRO_CONFIG * tempConfig = al_load_config_file(iniFile.data());
    if(tempFloat != (float)0)
    {
        char tempChar[15];
        sprintf(tempChar,"%f",tempFloat);
        al_set_config_value(tempConfig, section.data(), name.data(), tempChar);

    }else
    {
        al_set_config_value(tempConfig, section.data(), name.data(), "0");
    }
    if(!al_save_config_file(iniFile.data(),tempConfig))
    {
        fprintf(stderr, "Error writing to the config file\n");
    }
    al_destroy_config(tempConfig);
}
void saveValue(int tempInt, std::string name, std::string section, std::string iniFile)
{
    ALLEGRO_CONFIG * tempConfig = al_load_config_file(iniFile.data());
    char tempChar[8];
    sprintf(tempChar,"%i",tempInt);
    al_set_config_value(tempConfig, section.data(), name.data(), tempChar);
    if(!al_save_config_file(iniFile.data(),tempConfig))
    {
        fprintf(stderr, "Error writing to the config file\n");
    }
    al_destroy_config(tempConfig);
}
getValue::getValue(std::string nameTemp, std::string sectionTemp, std::string iniFileTemp)
{
    name=nameTemp;
    section=sectionTemp;
    iniFile=iniFileTemp;
    ALLEGRO_CONFIG * tempConfig = al_load_config_file(iniFile.data());
    isNumber = exists = false;
    const char * tempChar =al_get_config_value(tempConfig,section.data(),name.data());
    if(tempChar != NULL)
    {
        std::string tempString = tempChar;
        if(!tempString.compare("0"))
        {
            isNumber = true;
            numValue = 0;
        }else if(numValue = std::atof(tempString.data()))
        {
            isNumber = true;
        }else
        {
            stringValue = tempString;
        }
        exists = true;
    }
    al_destroy_config(tempConfig);
}
void getValue::update(std::string nameTemp, std::string sectionTemp, std::string iniFileTemp)
{
    name=nameTemp;
    section=sectionTemp;
    iniFile=iniFileTemp;
    ALLEGRO_CONFIG * tempConfig = al_load_config_file(iniFile.data());
    isNumber = exists = false;
    const char * tempChar =al_get_config_value(tempConfig,section.data(),name.data());
    if(tempChar != NULL)
    {
        std::string tempString = tempChar;
        if(!tempString.compare("0"))
        {
            isNumber = true;
            numValue = 0;
        }else if(numValue = std::atof(tempString.data()))
        {
            isNumber = true;
        }else
        {
            stringValue = tempString;
        }
        exists = true;
    }
    al_destroy_config(tempConfig);
}
