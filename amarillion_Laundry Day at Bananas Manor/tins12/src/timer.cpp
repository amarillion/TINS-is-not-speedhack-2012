#include "timer.h"
#include <stdio.h>

int Timer::update()
{
	IComponent::update();
	if (getCounter() == maxCounter)
	{
		target->handleMessage(msg);
	}
	if (getCounter() >= maxCounter)
	{
		kill(); // timer kills itself
	}
	return 0;
}

Timer::~Timer()
{
	printf("Timer deleted\n");
}
