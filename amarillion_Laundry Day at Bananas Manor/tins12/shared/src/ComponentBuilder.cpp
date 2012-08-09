/*
 * componentBuilder.cpp
 *
 *  Created on: 3 Aug 2012
 *      Author: martijn
 */

#include "ComponentBuilder.h"
#include "component.h"
#include "IContainer.h"
#include "motionimpl.h"
#include <allegro.h>
#include <stdarg.h>
#include <stdio.h>
#include <string>
#include <text.h>
#include "anim.h"

using namespace std;
using namespace boost;

ComponentBuilder::ComponentBuilder()
{
	component = shared_ptr<IComponent>(new IComponent());
}

ComponentBuilder &ComponentBuilder::clearScreen(int color)
{
	VisPtr temp = VisPtr(new ClearScreen(color));
	component->setVis(temp);
	return *this;
}

ComponentBuilder &ComponentBuilder::pattern(BITMAP *bmp)
{
	VisPtr temp = VisPtr(new Pattern(bmp));
	component->setVis(temp);
	return *this;
}

ComponentBuilder &ComponentBuilder::motion(IMotionPtr motion)
{
	component->setMotion(motion);
	return *this;
}

ComponentBuilder &ComponentBuilder::linear(int x, int y)
{
	IMotionPtr temp = IMotionPtr(new Linear (x, y));
	component->setMotion(temp);
	return *this;
}

void ComponentBuilder::addTo(IContainer *ic, int zOrder)
{
	ic->add(component, true, zOrder);
}

ComponentBuilder & ComponentBuilder::text(std::string val)
{
	VisPtr temp = VisPtr(new Text(val));
	component->setVis(temp);
	return *this;
}

ComponentBuilder & ComponentBuilder::textf(const char *msg, ...)
{
	char buf[256];

	va_list ap;
	va_start(ap, msg);
	vsnprintf (buf, sizeof(buf), msg, ap);
	va_end(ap);
	VisPtr temp = VisPtr(new Text(buf));
	component->setVis(temp);
	return *this;
}

ComponentBuilder & ComponentBuilder::center()
{
	component->setx(double(SCREEN_W / 2));
	component->sety(double(SCREEN_H / 2));
	return *this;
}

ComponentBuilder & ComponentBuilder::xy(double x, double y)
{
	component->setx(x);
	component->sety(y);
	return *this;
}

IComponentPtr ComponentBuilder::get()
{
	return component;
}

ComponentBuilder &ComponentBuilder::vis(VisPtr strategy)
{
	component->setVis(strategy);
	return *this;
}

