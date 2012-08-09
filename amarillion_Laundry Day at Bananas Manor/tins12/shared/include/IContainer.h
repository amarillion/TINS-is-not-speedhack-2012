/*
 * IContainer.h
 *
 *  Created on: 3 Aug 2012
 *      Author: martijn
 */

#ifndef ICONTAINER_H_
#define ICONTAINER_H_

#include "DrawStrategy.h"
#include <list>
#include "component.h"
#include "ComponentBuilder.h"

class IContainer : public IComponent
{
protected:
	std::list<IComponentPtr> children;
	IComponentPtr focus;
public:
	IContainer();
	void add (IComponentPtr item, bool active = true, int zOrder = 0);
	virtual void draw(const GraphicsContext &gc);
	virtual int update();
	void setFocus(IComponentPtr _focus);

	void setTimer(int msec, int event);
	virtual void purge();
	virtual void killAll();
	ComponentBuilder buildComponent(int zOrder = 0);
};

typedef boost::shared_ptr<IContainer> IContainerPtr;

#endif /* ICONTAINER_H_ */
