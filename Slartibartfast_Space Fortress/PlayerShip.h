#ifndef PlayerShip_h__
#define PlayerShip_h__

#include "SpaceShip.h"
class PlayerShip :
	public SpaceShip
{
public:
	PlayerShip(void);
	virtual ~PlayerShip(void);

	VOID KeyEvent(const ALLEGRO_EVENT &tEvent);
	virtual DOUBLE _Speed(){return 120;}
};

#endif // PlayerShip_h__