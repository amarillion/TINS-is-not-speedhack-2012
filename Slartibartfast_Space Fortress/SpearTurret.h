#ifndef SpearTurret_h__
#define SpearTurret_h__

#include "Common.h"
#include "Weapons.h"
#include "Turret.h"

class SpearTurret : public Turret
{
public:
	typedef enum SDir
	{
		SDIR_LEFT,
		SDIR_RIGHT,
		SDIR_UP,
		SDIR_DOWN
	};

	SpearTurret(IWeaponAdder *ptAdder, SDir eDir, BOOL bPlayerOwned);
	~SpearTurret(void);

	virtual
	VOID
	Draw() const;

protected:
	virtual VOID _Shoot();
	virtual DOUBLE _MaxCoolDown();
	virtual DOUBLE _Shield() const{return 0;}
	virtual DOUBLE _MaxLife() const{return 100;}

	SDir m_eDir;
};

#endif // SpearTurret_h__
