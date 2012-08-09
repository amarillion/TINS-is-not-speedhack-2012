/*
 * IContainer.cpp
 *
 *  Created on: 3 Aug 2012
 *      Author: martijn
 */

#include "IContainer.h"
#include "timer.h"

using namespace std;
using namespace boost;

IContainer::IContainer() { }

void IContainer::add (boost::shared_ptr<IComponent> item, bool enabled, int zOrder)
{
	if (zOrder < 0)
		children.push_front(item);
	else
		children.push_back (item);
	item->setAwake(enabled);
	item->setVisible(enabled);
	item->setFont(sfont);
	item->setViewport(viewPort);
}

int IContainer::update()
{
	IComponent::update();
	int result = 0;

	list<shared_ptr<IComponent> >::iterator i;
	for (i = children.begin(); i != children.end(); ++i)
	{
		if ((*i)->isAwake())
		{
			int retcode = (*i)->update();
			// this is a bit of a hack. Not a good way to guarantee that retcode is passed upstream.
			if (retcode != 0) result = retcode;
		}
	}

	purge();

	return result;
}

void IContainer::draw(const GraphicsContext &gc)
{
	list<shared_ptr<IComponent> >::iterator i;
	for (i = children.begin(); i != children.end(); ++i)
	{
		if ((*i)->isVisible())
			(*i)->draw(gc);
	}

	if (vis)
		vis->draw(gc);
}

void IContainer::setFocus(IComponentPtr _focus)
{
	if (focus)
	{
		focus->handleMessage(MSG_UNFOCUS);
	}
	focus = _focus;
	focus->handleMessage(MSG_FOCUS);
}

void IContainer::setTimer(int msec, int event)
{
	TimerPtr timer = TimerPtr(new Timer(msec, this, event));
	add (dynamic_pointer_cast<IComponent>(timer));
}

class MyComponentRemover
{
public:
	bool operator()(IComponentPtr o)
	{
		if (!o->isAlive())
		{
			o.reset();
			return 1;
		}
		return 0;
	}
};

void IContainer::purge()
{
	// remove all that are not alive!
	children.remove_if (MyComponentRemover());
}

void IContainer::killAll()
{
	list<IComponentPtr>::iterator i;
	for (i = children.begin(); i != children.end(); ++i)
	{
		(*i)->kill();
	}
	purge();
}

ComponentBuilder IContainer::buildComponent(int zOrder)
{
	ComponentBuilder result = ComponentBuilder();
	result.addTo(this, zOrder);
	return result;
}
