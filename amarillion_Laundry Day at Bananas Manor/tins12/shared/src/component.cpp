#include "component.h"
#include "DrawStrategy.h"
#include <stdio.h>

int IComponent::destructorCount = 0;
int IComponent::constructorCount = 0;

void IComponent::draw(const GraphicsContext &gc)
{
	if (!(alive && visible)) return;
	if (vis)
		vis->draw(gc);
}

IComponent::~IComponent()
{
	destructorCount++;
}

void IComponent::printDebugInfo()
{
	printf ("Constructor#: %i,  Destructor #: %i\n", constructorCount, destructorCount);
}

void IComponent::handleMessage(int msg)
{
	switch (msg)
	{
	case MSG_FOCUS:
		awake = true;
		visible = true;
		break;
	case MSG_UNFOCUS:
		awake = false;
		visible = false;
		break;
	case MSG_KILL:
		kill();
		break;
	}
}
