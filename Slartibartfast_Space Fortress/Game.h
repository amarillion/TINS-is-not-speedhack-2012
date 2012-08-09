#ifndef Game_h__
#define Game_h__

#include "Common.h"
#include "Weapons.h"
#include "Turret.h"
#include "Dragonship.h"
#include "PlayerShip.h"
#include "DefenceTurret.h"
#include "SpearTurret.h"
#include "CannonTurret.h"

class Game : public IWeaponAdder
{
public:
	Game(void);
	~Game(void);

	virtual VOID AddWeapon(Weapon *ptShot);

	VOID Start(DWORD dwStartingLevel);

protected:
	typedef enum TurretType
	{
		TYPE_SHIELD,
		TYPE_CANNON,
		TYPE_HARPOON
	};

	TurretType _GetType(DWORD dwIndex);

	VOID _Logic(DOUBLE dTimeDelta);

	VOID _Draw();

	VOID _ResolveCollisions();

	VOID _CullDead();

	VOID _StartLoop();

	VOID _LoadLevel(DWORD dwLevel);

	BOOL _IsPrime(DWORD dwNum);

	VOID _AddShip(BOOL bPlayer, DWORD dwStrength);

	DWORD _GetNextPrime(DWORD dwPrime);

	VOID _GetNextLocation(int &iX, int &iY);

	VOID _GenerateBG();

private:
	map<DWORD, Weapon *> m_aShots;
	map<DWORD, Dragonship *> m_aEnemies;
	PlayerShip *m_ptPlayer;
	DWORD m_dwCurrentLevel;
	DOUBLE m_dGameLen;
	ALLEGRO_BITMAP *m_ptSceneryBG0;
	ALLEGRO_BITMAP *m_ptSceneryBG1;
};

#endif // Game_h__


