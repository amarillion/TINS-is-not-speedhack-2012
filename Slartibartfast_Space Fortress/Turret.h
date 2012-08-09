#ifndef Turret_h__
#define Turret_h__

#include "Common.h"
#include "Collidable.h"
#include "Weapons.h"

class IWeaponAdder
{
public:
	virtual VOID AddWeapon(Weapon *ptShot) = 0;
};

class Turret
{
public:
	Turret(IWeaponAdder *ptAdder, BOOL bPlayerOwned);
	virtual~Turret(void);

	virtual
	VOID
	Draw() const;

	virtual
	VOID
	Shoot();

	virtual
	VOID
	Logic(DOUBLE dTimeDelta);
	
	VOID
	Move(
		const Rect &tNewPlace
	);

	BOOL
	IsAlive() const;

	VOID
	Hit(
		DOUBLE dDamage, 
		DOUBLE dPenetration
	);

protected:
	virtual VOID _Shoot() = 0;
	virtual DOUBLE _MaxCoolDown() = 0;
	virtual DOUBLE _Shield() const = 0;
	virtual DOUBLE _MaxLife() const = 0;

	VOID _DrawRect(const Rect &tShape, DOUBLE dAlphaMod)const;

protected:
	Rect m_tShape;
	Rect m_tLastShape;
	DOUBLE m_dLastLogic;
	DOUBLE m_dCoolDown;
	DOUBLE m_dLife;
	BOOL m_bPlayerOwned;
	IWeaponAdder *m_ptAdder;
};



#endif // Turret_h__