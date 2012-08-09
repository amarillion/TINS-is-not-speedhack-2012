#ifndef MAINLOOP_H
#define MAINLOOP_H

#include <allegro.h>
#include "component.h"
#include "sound.h"
#include "timer.h"

#include <string>
#include <vector>

class MainLoop : public IComponent, public Sound
{
private:
	BITMAP *buffer;
	
	const char *configFilename;
	const char *title;
	
	int frame_count;
	int frame_counter;
	int last_fps;
	int lastUpdateMsec;

	// game options
	bool fpsOn;
	bool windowed;
	bool stretch;
	
	static void timer_handler();
	static volatile int counterMsec;
	
	void getFromConfig();
	void getFromArgs(int argc, const char *const *argv);

	std::vector<int> resX;
	std::vector<int> resY;
	int bufw;
	int bufh;
	int logicIntervalMsec;
protected:
	bool quit;
public:
	std::vector<std::string> options;
	
	void addResolution (int w, int h);
	
	int getCounter () { return counterMsec; }
	
	MainLoop (const char *configFilename, const char *title, int _bufw = 640, int _bufh = 480);
	int init(int argc, const char *const *argv);
	void run();	
	virtual ~MainLoop();
	
	virtual int postInit() = 0;
	virtual void parseOpts(std::vector<std::string> &opts) = 0;
	void setLogicIntervalMsec (int value) { logicIntervalMsec = value; }
	int getLogicIntervalMsec () { return logicIntervalMsec; }
};

#endif
