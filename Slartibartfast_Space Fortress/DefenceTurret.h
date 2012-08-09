#ifndef DEFENCETURRET_H__
#define DEFENCETURRET_H__
#include "Turret.h"

class DefenceTurret :
	public Turret
{
public:
	DefenceTurret(IWeaponAdder *ptAdder, BOOL bPlayerOwned);
	virtual ~DefenceTurret(void);

	virtual
	VOID
	Draw() const;

protected:
	virtual VOID _Shoot(){return;}
	virtual DOUBLE _MaxCoolDown(){return 100000000;}
	virtual DOUBLE _Shield() const{return 10;}
	virtual DOUBLE _MaxLife() const{return 100;}
};

#endif