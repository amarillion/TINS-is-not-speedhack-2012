#include "Dragonship.h"


Dragonship::Dragonship(void):SpaceShip(CollisionManager::PhysObject::COLLISION_GROUP::GROUP_ENEMY),dLifeTime(0)
{
}


Dragonship::~Dragonship(void)
{
}

VOID Dragonship::Logic(DOUBLE dTimeDelta)
{
	m_bShoot = TRUE;
	dLifeTime += dTimeDelta;

	DOUBLE dTargetX = (m_ptPhysObj->m_aRects.size() * 100 + 100) % 1024;
	DOUBLE dTargetY = (((m_ptPhysObj->m_aRects.size() * 100)/1000)*10 + 100);

	if (m_ptPhysObj->m_ptDistanceCircle->m_dCX < dTargetX - 50)
	{
		m_bLeft = FALSE;
		m_bRight = TRUE;
	}
	if (m_ptPhysObj->m_ptDistanceCircle->m_dCX > dTargetX + 50)
	{
		m_bLeft = TRUE;
		m_bRight = FALSE;
	}
	if (m_ptPhysObj->m_ptDistanceCircle->m_dCY < dTargetY - 10)
	{
		m_bUp = FALSE;
		m_bDown = TRUE;
	}
	else if (m_ptPhysObj->m_ptDistanceCircle->m_dCY > dTargetY + 10)
	{
		m_bUp = TRUE;
		m_bDown = FALSE;
	}
	else
	{
		m_bUp = FALSE;
		m_bDown = FALSE;
	}

	if ((!m_bLeft) && (!m_bRight))
	{
		m_bLeft = TRUE;
	}

	SpaceShip::Logic(dTimeDelta);
}

DOUBLE Dragonship::_Speed()
{
	return 80 + m_ptPhysObj->m_aRects.size()*10;
}