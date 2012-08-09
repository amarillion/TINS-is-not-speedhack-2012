#ifndef MAIN_H
#define MAIN_H

#include <allegro.h>
#include "resources.h"
#include "engine.h"
#include "settings.h"
#include "mainloop.h"

class Main : public MainLoop
{
private:
	Engine engine;
	Settings settings;
	Resources resources;
	
public:

	Settings *getSettings() { return &settings; }
	Resources *getResources() { return &resources; }
	virtual int postInit();
	virtual int update();
	virtual void draw (const GraphicsContext &gc);
	virtual void parseOpts(std::vector<std::string> &opts) {}
	virtual void handleMessage(int code) {}
	void stop();
	Main ();
	~Main();
};

#endif
