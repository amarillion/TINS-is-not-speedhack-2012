#ifndef Weapons_h__
#define Weapons_h__

#include "Common.h"
#include "Collidable.h"

class Weapon
{
public:
	Weapon(void);
	virtual ~Weapon(void);

	virtual VOID Draw()const = 0;
	virtual VOID Logic(DOUBLE dTimeDelta) = 0;

	virtual DOUBLE GetDamage()const = 0;
	virtual DOUBLE GetPenetration()const = 0;
	virtual BOOL IsAlive()const = 0;
	VOID Kill(){m_bKilled = TRUE;}

	CollisionManager::PhysObject *const GetPhysObj(){return m_ptPhysObj;};

protected:
	BOOL m_bKilled;
	CollisionManager::PhysObject *m_ptPhysObj;
};

class Harpoon : public Weapon
{
public:
	Harpoon(
		CollisionManager::PhysObject::COLLISION_GROUP eGroup, 
		DOUBLE dCX, 
		DOUBLE dCY,
		DOUBLE dXSpeed,
		DOUBLE dYSpeed
	);
	virtual ~Harpoon();

	virtual VOID Draw()const;
	virtual VOID Logic(DOUBLE dTimeDelta);
	virtual BOOL IsAlive()const;

	virtual DOUBLE GetDamage()const {return 10.0;};
	virtual DOUBLE GetPenetration()const {return 0.2;};

protected:
	DOUBLE m_dXSpeed;
	DOUBLE m_dYSpeed;
	DOUBLE m_dLife;
};

class Cannonball : public Weapon
{
public:
	Cannonball(
		CollisionManager::PhysObject::COLLISION_GROUP eGroup, 
		DOUBLE dCX, 
		DOUBLE dCY,
		DOUBLE dXSpeed,
		DOUBLE dYSpeed
	);
	virtual ~Cannonball();

	virtual VOID Draw()const;
	virtual VOID Logic(DOUBLE dTimeDelta);
	virtual BOOL IsAlive()const;

	virtual DOUBLE GetDamage()const {return 20.0;};
	virtual DOUBLE GetPenetration()const {return 0.0;};

protected:
	DOUBLE m_dXSpeed;
	DOUBLE m_dYSpeed;
	
};

#endif // Weapons_h__
