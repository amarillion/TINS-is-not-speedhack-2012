#ifndef MOTION_H
#define MOTION_H

#include <boost/shared_ptr.hpp>

class IMotion
{
public:
	virtual ~IMotion() { }
	virtual int getdx(int counter) = 0;
	virtual int getdy(int counter) = 0;
};

typedef boost::shared_ptr<IMotion> IMotionPtr;

#endif
