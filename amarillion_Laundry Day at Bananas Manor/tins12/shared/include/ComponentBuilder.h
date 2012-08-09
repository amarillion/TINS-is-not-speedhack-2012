/*
 * componentBuilder.h
 *
 *  Created on: 3 Aug 2012
 *      Author: martijn
 */

#ifndef COMPONENTBUILDER_H_
#define COMPONENTBUILDER_H_

#include <boost/shared_ptr.hpp>
#include "motion.h"
#include "DrawStrategy.h"

class IContainer;
class IComponent;
struct BITMAP;

class ComponentBuilder {
public:
	ComponentBuilder();

	ComponentBuilder &clearScreen(int Color);
	ComponentBuilder &pattern(BITMAP *bmp);
	ComponentBuilder &motion(IMotionPtr motion);
	ComponentBuilder &linear(int x, int y);
	ComponentBuilder &vis(VisPtr strategy);
	void addTo(IContainer *ic, int zOrder = 0);
	boost::shared_ptr<IComponent> get();
	ComponentBuilder & text(std::string val);
	ComponentBuilder & textf(const char *msg, ...);
	ComponentBuilder & center();
	ComponentBuilder & xy(double x, double y);
private:
	boost::shared_ptr<IComponent> component;
};

#endif /* COMPONENTBUILDER_H_ */
