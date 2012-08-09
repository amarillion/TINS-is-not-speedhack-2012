#ifndef life5_getValue_hpp
#define life5_getValue_hpp
#include <string>
#include <allegro5/allegro.h>
#include <cstdlib>
#include <fstream>
class getValue
{
    public:
    getValue() {};
    getValue(std::string name, std::string section, std::string iniFile);
    void update(std::string name, std::string section, std::string iniFile);
    bool isNumber;
    bool exists;
    double numValue;
    std::string stringValue;
    std::string name,section,iniFile;
};
void saveValue(int tempInt, std::string name, std::string section, std::string iniFile);
void saveValue(float tempFloat, std::string name, std::string section, std::string iniFile);
#endif
