#include "SpaceShip.h"


SpaceShip::SpaceShip(CollisionManager::PhysObject::COLLISION_GROUP eGroup)
{
	DWORD dwIgnore;
	memset(this, 0, sizeof(*this));
	m_ptPhysObj = CollisionManager::New(eGroup, dwIgnore);
	m_ptPhysObj->m_ptDistanceCircle = new Circle();
	m_ptPhysObj->m_ptDistanceCircle->m_dR = sqrt((float)TURRET_HEIGHT * TURRET_HEIGHT + TURRET_WIDTH * TURRET_WIDTH);
	if (CollisionManager::PhysObject::COLLISION_GROUP::GROUP_PLAYER == eGroup)
	{
		m_ptPhysObj->m_ptDistanceCircle->m_dCX = 600;
		m_ptPhysObj->m_ptDistanceCircle->m_dCY = 700;
	}
	else
	{
		m_ptPhysObj->m_ptDistanceCircle->m_dCX = 200 + rand()%512;
		m_ptPhysObj->m_ptDistanceCircle->m_dCY = 100;
	}
	
}


SpaceShip::~SpaceShip(void)
{
	CollisionManager::Delete(m_ptPhysObj->m_dwObjId);
}

VOID
SpaceShip::_Shoot()
{
	for (int i=0; i < m_aTurrets.size(); ++i)
	{
		m_aTurrets[i].second->Shoot();
	}
}

VOID SpaceShip::_UpdatePlace(DOUBLE dTimeDelta)
{
	DOUBLE dDX = 0;
	DOUBLE dDY = 0;

	if (m_bLeft)
	{
		dDX -= dTimeDelta * _Speed();
	}
	if (m_bRight)
	{
		dDX += dTimeDelta * _Speed();
	}
	if (m_bUp)
	{
		dDY -= dTimeDelta * _Speed();
	}
	if (m_bDown)
	{
		dDY += dTimeDelta * _Speed();
	}
	m_ptPhysObj->Move(dDX, dDY);
}
VOID SpaceShip::_UpdateRectangle()
{
	for (int i=0; i < m_aTurrets.size(); ++i)
	{
		m_aTurrets[i].second->Move(m_ptPhysObj->m_aRects[i]);
	}
}
VOID SpaceShip::_RemoveDeadTurrets()
{
	for (int i=0; i<m_aTurrets.size(); ++i)
	{
		Turret *ptTurret = m_aTurrets[i].second;
		if (!ptTurret->IsAlive())
		{
			m_ptPhysObj->m_aRects[i] = m_ptPhysObj->m_aRects[m_ptPhysObj->m_aRects.size()-1];
			m_ptPhysObj->m_aRects.pop_back();
			m_aTurrets[i] = m_aTurrets[m_aTurrets.size() - 1];
			m_aTurrets.pop_back();
			DELETEPTR(ptTurret);
		}
	}
}

VOID
SpaceShip::Logic(DOUBLE dTimeDelta)
{
	_RemoveDeadTurrets();

	for (int i=0 ;i<m_aTurrets.size(); ++i)
	{
		m_aTurrets[i].second->Logic(dTimeDelta);
	}

	_UpdatePlace(dTimeDelta);

	_UpdateRectangle();

	if (m_bShoot)
	{
		_Shoot();
	}
}

VOID
SpaceShip::Hit(
	const CollisionManager::Collision &tCollision,
	DOUBLE dDamage, 
	DOUBLE dPenetration
)
{
	DWORD dwIndex = 0;
	if (m_ptPhysObj->m_dwObjId == tCollision.dwObjId0)
	{
		dwIndex = tCollision.dwObj0Index;
	}
	else if (m_ptPhysObj->m_dwObjId == tCollision.dwObjId1)
	{
		dwIndex = tCollision.dwObj1Index;
	}
	else
	{
		return;
	}

	Turret *ptTurret = m_aTurrets[dwIndex].second;

	ptTurret->Hit(dDamage, dPenetration);
}

VOID
SpaceShip::_AddTurret(Turret *ptTurret, int iLocX, int iLocY)
{
	Rect tShape;
	tShape.m_dCX = m_ptPhysObj->m_ptDistanceCircle->m_dCX + (iLocX) * (TURRET_WIDTH + 2) - 0.5*TURRET_WIDTH;
	tShape.m_dCY = m_ptPhysObj->m_ptDistanceCircle->m_dCY + (iLocY) * (TURRET_HEIGHT + 2) - 0.5*TURRET_HEIGHT;
	tShape.m_dWidth = TURRET_WIDTH;
	tShape.m_dHeight = TURRET_HEIGHT;
	
	m_ptPhysObj->m_aRects.push_back(tShape);
	m_aTurrets.push_back(pair<pair<int, int>, Turret *>(pair<int, int>(iLocX, iLocY) , ptTurret));

	DOUBLE dDistance = 	sqrt((tShape.m_dCX - m_ptPhysObj->m_ptDistanceCircle->m_dCX) *
						(tShape.m_dCX - m_ptPhysObj->m_ptDistanceCircle->m_dCX) +
						(tShape.m_dCY - m_ptPhysObj->m_ptDistanceCircle->m_dCY) *
						(tShape.m_dCY - m_ptPhysObj->m_ptDistanceCircle->m_dCY) +
						MAX(TURRET_HEIGHT, TURRET_WIDTH));

	m_ptPhysObj->m_ptDistanceCircle->m_dR = MAX(m_ptPhysObj->m_ptDistanceCircle->m_dR, dDistance);

	if (1 == m_aTurrets.size())
	{
		m_ptPhysObj->m_ptDistanceCircle->m_dCX = tShape.m_dCX + 0.5  * tShape.m_dWidth;
		m_ptPhysObj->m_ptDistanceCircle->m_dCY = tShape.m_dCY + 0.5  * tShape.m_dHeight;
	}
}

VOID SpaceShip::Draw() const
{
	for (int i=0; i< m_aTurrets.size(); ++i)
	{
		m_aTurrets[i].second->Draw();
	}
}

BOOL SpaceShip::IsAlive()
{
	return (0 != m_aTurrets.size());
}