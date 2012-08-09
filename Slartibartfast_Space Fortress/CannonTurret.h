#ifndef CannonTurret_h__
#define CannonTurret_h__

#include "Turret.h"
class CannonTurret :
	public Turret
{
public:
	typedef enum CDir
	{
		CDIR_UP,
		CDIR_DOWN
	};

	CannonTurret(IWeaponAdder *ptAdder, CDir eDir, BOOL bPlayerOwned);
	virtual ~CannonTurret(void);

	virtual
	VOID
	Draw() const;

protected:
	virtual VOID _Shoot();
	virtual DOUBLE _MaxCoolDown() {return (m_bPlayerOwned)?(1.5):(3.0);}
	virtual DOUBLE _Shield() const{return 5;}
	virtual DOUBLE _MaxLife() const{return 80;}

	CDir m_eDir;
};

#endif // CannonTurret_h__