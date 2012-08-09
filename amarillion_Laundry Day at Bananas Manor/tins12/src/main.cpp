#include <assert.h>
#include "main.h"
#include "color.h"
#include "mainloop.h"
#include <alfont.h>

using namespace boost;

Main::Main() : MainLoop ("tins12.cfg", "Laundry day at bananas manor", 800, 480), engine(this), settings(), resources()
{
	addResolution (800, 480);
	addResolution (768, 480);
	addResolution (640, 480);
}

const int DIRNUM = 2;
const char *DIRECTIONS[DIRNUM] = { "left", "right" };

int Main::postInit()
{
	shared_ptr<IDirectionModel> model(new DirectionModel(DIRECTIONS, DIRNUM));
	Anim::setDirectionModel (model);

	settings.getFromConfig();
	initKeyboard(); // install custom keyboard handler of Button.cpp

	if (!(
		resources.addFiles("data/*.ttf") &&
		resources.addFiles("data/*.bmp") &&
		resources.addFiles("data/*.xml") &&
		resources.addFiles("data/*.ogg") &&
		resources.addFiles("data/*.dat")
		))
	{
		allegro_message ("Error while loading resources!\n%s", resources.getErrorMsg());
		return 0;
	}

	if (!(
		resources.addMapFile("bg1", "data/bg1.map", "tiles1") &&
		resources.addMapFile("map1", "data/map1.map", "tiles1") &&
		resources.addMapFile("map2", "data/map2.map", "tiles1") &&
		resources.addMapFile("map3", "data/map3.map", "tiles1") &&
		resources.addMapFile("map4", "data/map4.map", "tiles1") &&
		resources.addMapFile("bg2", "data/bg2.map", "tiles1") &&
		resources.addMapFile("bg3", "data/bg3.map", "tiles1") &&
		resources.addMapFile("bg4", "data/bg4.map", "tiles1")
	))
	{
		allegro_message ("Error while loading resources!\n%s", resources.getErrorMsg());
		return 0;
	}

	engine.setFont(resources.getAlfont("megaman_2", 24));
	engine.init();
	return 1;
}

Main::~Main()
{
	engine.done();
}

int Main::update()
{
	return engine.update();
}

void Main::draw (const GraphicsContext &gc)
{
	engine.draw(gc);
}

void Main::stop()
{
	quit = true;
}

int main(int argc, const char *const *argv)
{
	{
		Main m;
		if (m.init(argc, argv))
		{
			m.run();
		}
	}
	IComponent::printDebugInfo();
	return 0;
} END_OF_MAIN();
