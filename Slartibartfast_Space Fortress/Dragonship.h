#pragma once
#include "spaceship.h"
class Dragonship :
	public SpaceShip
{
public:
	Dragonship(void);
	virtual ~Dragonship(void);

	virtual VOID Logic(DOUBLE dTimeDelta);

	virtual DOUBLE _Speed();

private:
	DOUBLE dLifeTime;
};

