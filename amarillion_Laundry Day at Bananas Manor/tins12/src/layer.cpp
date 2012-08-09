#include "layer.h"
#include "engine.h"
#include "color.h"
#include <math.h>

void Layer::background()
{
	awake = true;
	visible = true;
}

void Layer::foreground()
{
	awake = true;
	visible = true;
}

void Layer::stop()
{
	awake = false;
	visible = false;
}

void Layer::draw(BITMAP * dest)
{
	// default implementation: clear blue
	clear_to_color (dest, BLUE);
}

Layer::Layer(Engine & _engine) : parent (_engine)
{
	awake = false;
}
