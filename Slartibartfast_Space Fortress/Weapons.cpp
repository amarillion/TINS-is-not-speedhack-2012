#include "Weapons.h"


Weapon::Weapon(void)
{
	m_bKilled = FALSE;
}


Weapon::~Weapon(void)
{
}

Harpoon::Harpoon(
	CollisionManager::PhysObject::COLLISION_GROUP eGroup, 
	DOUBLE dCX, 
	DOUBLE dCY, 
	DOUBLE dXSpeed,
	DOUBLE dYSpeed
):m_dXSpeed(dXSpeed),m_dYSpeed(dYSpeed),m_dLife(0)
{
	DWORD dwIgnore = 0;
	m_ptPhysObj = CollisionManager::New(eGroup, dwIgnore);
	m_ptPhysObj->m_ptDistanceCircle = new Circle();
	m_ptPhysObj->m_ptDistanceCircle->m_dCX = dCX;
	m_ptPhysObj->m_ptDistanceCircle->m_dCY = dCY;
	m_ptPhysObj->m_ptDistanceCircle->m_dR = 1.0;
	m_ptPhysObj->m_aCircles.push_back(*m_ptPhysObj->m_ptDistanceCircle);
}

Harpoon::~Harpoon()
{
	if (NULL != m_ptPhysObj)
	{
		CollisionManager::Delete(m_ptPhysObj->m_dwObjId);
	}
}

VOID
Harpoon::Draw()const
{
	DOUBLE dAngle = atan2(m_dYSpeed, m_dXSpeed);
	DOUBLE dLen = sqrt(m_dXSpeed*m_dXSpeed + m_dYSpeed*m_dYSpeed);
	al_draw_line(	m_ptPhysObj->m_ptDistanceCircle->m_dCX,
					m_ptPhysObj->m_ptDistanceCircle->m_dCY,
					m_ptPhysObj->m_ptDistanceCircle->m_dCX - 0.15*m_dXSpeed,
					m_ptPhysObj->m_ptDistanceCircle->m_dCY - 0.15*m_dYSpeed,
					al_map_rgba(200, 200, 200, 200),
					1.0);
	al_draw_line(	m_ptPhysObj->m_ptDistanceCircle->m_dCX,
					m_ptPhysObj->m_ptDistanceCircle->m_dCY,
					m_ptPhysObj->m_ptDistanceCircle->m_dCX - 0.05*dLen*cos(dAngle + 0.30),
					m_ptPhysObj->m_ptDistanceCircle->m_dCY - 0.05*dLen*sin(dAngle + 0.30),
					al_map_rgba(200, 200, 200, 200),
					1.0);
	al_draw_line(	m_ptPhysObj->m_ptDistanceCircle->m_dCX,
					m_ptPhysObj->m_ptDistanceCircle->m_dCY,
					m_ptPhysObj->m_ptDistanceCircle->m_dCX - 0.05*dLen*cos(dAngle - 0.30),
					m_ptPhysObj->m_ptDistanceCircle->m_dCY - 0.05*dLen*sin(dAngle - 0.30),
					al_map_rgba(200, 200, 200, 200),
					1.0);
}
VOID
Harpoon::Logic(DOUBLE dTimeDelta)
{
	m_ptPhysObj->Move(m_dXSpeed*dTimeDelta, m_dYSpeed*dTimeDelta);
	m_dLife += dTimeDelta;
}

BOOL Harpoon::IsAlive()const
{
	if (m_bKilled)
		return FALSE;
	return (m_dLife < 6);
}

Cannonball::Cannonball(
	CollisionManager::PhysObject::COLLISION_GROUP eGroup, 
	DOUBLE dCX, 
	DOUBLE dCY, 
	DOUBLE dXSpeed,
	DOUBLE dYSpeed
):m_dXSpeed(dXSpeed),m_dYSpeed(dYSpeed)
{
	DWORD dwIgnore = 0;
	m_ptPhysObj = CollisionManager::New(eGroup, dwIgnore);
	m_ptPhysObj->m_ptDistanceCircle = new Circle();
	m_ptPhysObj->m_ptDistanceCircle->m_dCX = dCX;
	m_ptPhysObj->m_ptDistanceCircle->m_dCY = dCY;
	m_ptPhysObj->m_ptDistanceCircle->m_dR = 3.0;
	m_ptPhysObj->m_aCircles.push_back(*m_ptPhysObj->m_ptDistanceCircle);
}

Cannonball::~Cannonball()
{
}

VOID
Cannonball::Draw()const
{
	al_draw_filled_circle(	m_ptPhysObj->m_ptDistanceCircle->m_dCX, 
							m_ptPhysObj->m_ptDistanceCircle->m_dCY, 
							m_ptPhysObj->m_ptDistanceCircle->m_dR, 
							al_map_rgba(200, 95, 95, 200));
}
VOID
Cannonball::Logic(DOUBLE dTimeDelta)
{
	m_ptPhysObj->Move(m_dXSpeed*dTimeDelta, m_dYSpeed*dTimeDelta);
	
	if (m_dXSpeed > 100 * dTimeDelta)
	{
		m_dXSpeed -= 100 * dTimeDelta;
	}
	else if (m_dXSpeed < -100 * dTimeDelta)
	{
		m_dXSpeed += 100 * dTimeDelta;
	}
	else
	{
		m_dXSpeed = 0;
	}

	if (m_dYSpeed > 100 * dTimeDelta)
	{
		m_dYSpeed -= 100 * dTimeDelta;
	}
	else if (m_dYSpeed < -100 * dTimeDelta)
	{
		m_dYSpeed += 100 * dTimeDelta;
	}
	else
	{
		m_dYSpeed = 0;
	}
}

BOOL Cannonball::IsAlive()const
{
	if (m_bKilled)
		return FALSE;
	return (MAX(ABS(m_dYSpeed),ABS(m_dXSpeed)) > 1);
}