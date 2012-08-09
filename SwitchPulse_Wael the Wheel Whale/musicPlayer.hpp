#ifndef life5_musicPlayer_hpp
#define life5_musicPlayer_hpp
#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include "getValue.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "splitter.cpp"
class Shortcut
{
    public:
    std::string name;
    std::string value;
};
class musicPlayer
{
    public:
    void update(ALLEGRO_EVENT ev);
    void init(std::string iniFile, std::string section, ALLEGRO_EVENT_QUEUE *event_queue);
    void start();
    std::string checkShortcut(std::vector<Shortcut> shortcuts, std::string name);
    std::vector<Shortcut> parseShortcuts(std::string filename);
    std::vector<ALLEGRO_SAMPLE*> parseTrack(std::string filename, std::vector<Shortcut> shortcuts, std::string instrument, int max);
    void destroy();
    private:
    ALLEGRO_TIMER *timerA;
    ALLEGRO_TIMER *timerB;
    ALLEGRO_TIMER *timerC;
    ALLEGRO_TIMER *timerD;
    ALLEGRO_TIMER *timerE;
    std::vector<ALLEGRO_SAMPLE *> trackA;
    std::vector<ALLEGRO_SAMPLE *> trackB;
    std::vector<ALLEGRO_SAMPLE *> trackC;
    std::vector<ALLEGRO_SAMPLE *> trackD;
    std::vector<ALLEGRO_SAMPLE *> trackE;
    int trackACurrentNote;
    int trackBCurrentNote;
    int trackCCurrentNote;
    int trackDCurrentNote;
    int trackECurrentNote;
	std::vector<std::vector<ALLEGRO_SAMPLE *> > sounds;
};
#endif
