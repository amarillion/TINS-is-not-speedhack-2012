#ifndef SpaceShip_h__
#define SpaceShip_h__

#include "Common.h"
#include "Collidable.h"
#include "Turret.h"

class SpaceShip
{
public:
	SpaceShip(CollisionManager::PhysObject::COLLISION_GROUP eGroup);
	~SpaceShip(void);

	virtual VOID Logic(DOUBLE dTimeDelta);

	virtual VOID Draw() const;

	VOID
	Hit(
		const CollisionManager::Collision &tCollision,
		DOUBLE dDamage, 
		DOUBLE dPenetration
	);

	DWORD GetObjId() const {return m_ptPhysObj->m_dwObjId;}

	BOOL IsAlive();

	VOID _AddTurret(Turret *ptTurret, int iLocX, int iLocY);

protected:
	VOID _Shoot();

	virtual DOUBLE _Speed(){return 80;}
	
	BOOL m_bLeft,m_bRight,m_bUp,m_bDown,m_bShoot;
	CollisionManager::PhysObject *m_ptPhysObj;

private:
	VOID _UpdatePlace(DOUBLE dTimeDelta);
	VOID _UpdateRectangle();
	VOID _RemoveDeadTurrets();

	vector<pair<pair<int, int>, Turret *> > m_aTurrets;
};

#endif // SpaceShip_h__