#include "musicPlayer.hpp"


void musicPlayer::init(std::string iniFile, std::string section, ALLEGRO_EVENT_QUEUE *event_queue)
{
    getValue aNum,bNum,cNum,dNum,eNum, trackAFilename,trackBFilename,trackCFilename,trackDFilename,trackEFilename, trackAFilenameShortcut,trackBFilenameShortcut,trackCFilenameShortcut,trackDFilenameShortcut,trackEFilenameShortcut;
    aNum.update("aSpeed",section,iniFile);
    bNum.update("bSpeed",section,iniFile);
    cNum.update("cSpeed",section,iniFile);
    dNum.update("dSpeed",section,iniFile);
    eNum.update("eSpeed",section,iniFile);
    trackAFilename.update("trackA",section,iniFile);
    trackBFilename.update("trackB",section,iniFile);
    trackCFilename.update("trackC",section,iniFile);
    trackDFilename.update("trackD",section,iniFile);
    trackEFilename.update("trackE",section,iniFile);
    trackAFilenameShortcut.update("trackAShortcut",section,iniFile);
    trackBFilenameShortcut.update("trackBShortcut",section,iniFile);
    trackCFilenameShortcut.update("trackCShortcut",section,iniFile);
    trackDFilenameShortcut.update("trackDShortcut",section,iniFile);
    trackEFilenameShortcut.update("trackEShortcut",section,iniFile);
    timerA = al_create_timer(1.0 / aNum.numValue);
    timerB = al_create_timer(1.0 / bNum.numValue);
    timerC = al_create_timer(1.0 / cNum.numValue);
    timerD = al_create_timer(1.0 / dNum.numValue);
    timerE = al_create_timer(1.0 / eNum.numValue);
    al_register_event_source(event_queue, al_get_timer_event_source(timerA));
    al_register_event_source(event_queue, al_get_timer_event_source(timerB));
    al_register_event_source(event_queue, al_get_timer_event_source(timerC));
    al_register_event_source(event_queue, al_get_timer_event_source(timerD));
    al_register_event_source(event_queue, al_get_timer_event_source(timerE));
    trackACurrentNote = 0;
    trackBCurrentNote = 0;
    trackCCurrentNote = 0;
    trackDCurrentNote = 0;
    trackECurrentNote = 0;
    aNum.update("aKey",section,iniFile);
    bNum.update("bKey",section,iniFile);
    cNum.update("cKey",section,iniFile);
    dNum.update("dKey",section,iniFile);
    eNum.update("eKey",section,iniFile);
    trackA = parseTrack(trackAFilename.stringValue,parseShortcuts(trackAFilenameShortcut.stringValue),"A",aNum.numValue);
    trackB = parseTrack(trackBFilename.stringValue,parseShortcuts(trackBFilenameShortcut.stringValue),"B",bNum.numValue);
    trackC = parseTrack(trackCFilename.stringValue,parseShortcuts(trackCFilenameShortcut.stringValue),"C",cNum.numValue);
    trackD = parseTrack(trackDFilename.stringValue,parseShortcuts(trackDFilenameShortcut.stringValue),"D",dNum.numValue);
    trackE = parseTrack(trackEFilename.stringValue,parseShortcuts(trackEFilenameShortcut.stringValue),"E",eNum.numValue);
}

void musicPlayer::start()
{
    al_start_timer(timerA);
    al_start_timer(timerB);
    al_start_timer(timerC);
    al_start_timer(timerD);
    al_start_timer(timerE);
}

std::string musicPlayer::checkShortcut(std::vector<Shortcut> shortcuts, std::string name)
{
    for(int i = 0; i < shortcuts.size(); i++)
      {
          if(shortcuts.at(i).name == name)
          {
              //std::cout << name << std::endl;
              return shortcuts.at(i).value;
          }
      }
      //std::cout << name << std::endl;
      return NULL;
}
// in main
std::vector<ALLEGRO_SAMPLE*> musicPlayer::parseTrack(std::string filename, std::vector<Shortcut> shortcuts, std::string instrument, int max)
{
    std::string buffer;
    std::string result;
	std::ifstream infile;
	infile.open (filename.c_str());
    while(!infile.eof()) // To get you all the lines.
    {
        getline(infile,buffer); // Saves the line in STRING.
        result += buffer; // Prints our STRING.
    }
	infile.close();
	Splitter split(result, ",");
	result = "";
	for(int i = 0; i < split.size(); i++)
	{
	    if(!(split[i] != "0"  and atoi(split[i].c_str()) == 0))
	    {
            result += split[i] + ",";
	    }
	    else
	    {
	        std::string shortcutValue = checkShortcut(shortcuts, split[i]);
	        if(shortcutValue != "")
	        {
	            result += shortcutValue;
	        }
	    }
	}
	split.reset(result, ",");
	std::vector<ALLEGRO_SAMPLE *> song;
	std::vector<ALLEGRO_SAMPLE *> samples;
	for(int i = 1; i <= max; i++)
	{
	    std::stringstream out;
        out << i;
        std::string num = out.str();
        std::string sampleName = "ogg/" + instrument + num + ".ogg";
        ALLEGRO_SAMPLE *temp = al_load_sample(sampleName.c_str());
        samples.push_back(temp);
	}
	sounds.push_back(samples);
	for(int i = 0; i < split.size(); i++)
	{
        //std::cout << atoi(split[i].c_str()) << " |" << split[i] << "| " << instrument << " " << samples.size() << std::endl;
	    if(split[i]!="0" and split[i]!="")
	    {
            song.push_back(samples.at(atoi(split[i].c_str())-1));
        }
        else if(split[i]!="")
        song.push_back(NULL);
	}
	return song;
}

std::vector<Shortcut> musicPlayer::parseShortcuts(std::string filename)
{
    std::string buffer;
    std::vector<Shortcut> result;
	std::ifstream infile(filename.c_str());
	if (infile.is_open())
    while(!infile.eof())
    {
        getline(infile,buffer);
        if((int)buffer.find("=") > 0 and (int)buffer.find("=") < buffer.size() - 1)
        {
            Shortcut temp;
            temp.name = buffer.substr(0,(int)buffer.find("="));
            temp.value = buffer.substr((int)buffer.find("=") + 1, buffer.size() - 1);
            result.push_back(temp);
        }
    }
	infile.close();
	return result;
}

void musicPlayer::update(ALLEGRO_EVENT ev)
{
    if(ev.type == ALLEGRO_EVENT_TIMER)
    {
        if(ev.timer.source == timerA)
        {
            if(trackA.at(trackACurrentNote) != NULL)
            al_play_sample(trackA.at(trackACurrentNote), 1, 0, 1,ALLEGRO_PLAYMODE_ONCE, NULL);
            trackACurrentNote++;
            if(trackACurrentNote == trackA.size())
            {
                trackACurrentNote = 0;
            }
        }
        if(ev.timer.source == timerB)
        {
            if(trackB.at(trackBCurrentNote) != NULL)
            al_play_sample(trackB.at(trackBCurrentNote), 1, 0, 1,ALLEGRO_PLAYMODE_ONCE, NULL);
            trackBCurrentNote++;
            if(trackBCurrentNote == trackB.size())
            {
                trackBCurrentNote = 0;
            }
        }
        if(ev.timer.source == timerC)
        {
            if(trackC.at(trackCCurrentNote) !=NULL)
            al_play_sample(trackC.at(trackCCurrentNote), 1, 0, 1,ALLEGRO_PLAYMODE_ONCE, NULL);
            trackCCurrentNote++;
            if(trackCCurrentNote == trackC.size())
            {
                trackCCurrentNote = 0;
            }
        }
        if(ev.timer.source == timerD)
        {
            if(trackD.at(trackDCurrentNote) != NULL)
            al_play_sample(trackD.at(trackDCurrentNote), 1, 0, 1,ALLEGRO_PLAYMODE_ONCE, NULL);
            trackDCurrentNote++;
            if(trackDCurrentNote == trackD.size())
            {
                trackDCurrentNote = 0;
            }
        }
        if(ev.timer.source == timerE)
        {
            if(trackE.at(trackECurrentNote) != NULL)
            al_play_sample(trackE.at(trackECurrentNote), 1, 0, 1,ALLEGRO_PLAYMODE_ONCE, NULL);
            trackECurrentNote++;
            if(trackECurrentNote == trackE.size())
            {
                trackECurrentNote = 0;
            }
        }
    }
}

void musicPlayer::destroy()
{
    al_destroy_timer(timerA);
    al_destroy_timer(timerB);
    al_destroy_timer(timerC);
    al_destroy_timer(timerD);
    al_destroy_timer(timerE);

    for(int i = 0; i < sounds.size(); i++)
    {
        for(int j = 0; j < sounds.at(i).size(); j++)
        {
            al_destroy_sample(sounds.at(i).at(j));
        }
    }
}
