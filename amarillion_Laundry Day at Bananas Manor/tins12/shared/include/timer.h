#ifndef TIMER_H
#define TIMER_H

#include "component.h"
#include <boost/shared_ptr.hpp>

class ITimer
{
public:
	virtual int getCounter() = 0;
};

class Timer : public IComponent
{
	int msg;
	IComponent* target; //TODO: use a shared ptr
	int maxCounter;
public:
	Timer(int maxCounter, IComponent* target, int msg) :
		msg(msg), target(target), maxCounter(maxCounter)
		{ setAwake(true); setVisible(false); }
	virtual ~Timer();
	virtual int update();
};

typedef boost::shared_ptr<Timer> TimerPtr;

#endif
