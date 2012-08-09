#include "CannonTurret.h"
#include "Weapons.h"


CannonTurret::CannonTurret(IWeaponAdder *ptAdder, CDir eDir, BOOL bPlayerOwned):Turret(ptAdder,bPlayerOwned),m_eDir(eDir)
{
	m_dLife = _MaxLife();
}


CannonTurret::~CannonTurret(void)
{
}

VOID
CannonTurret::Draw() const
{
	ALLEGRO_COLOR tCol = al_map_rgba(200, 95, 20, 200 * m_dLife / _MaxLife());
	DOUBLE dCX = m_tShape.m_dCX + 0.5 * m_tShape.m_dWidth;
	DOUBLE dCY = m_tShape.m_dCY + 0.5 * m_tShape.m_dHeight;
	DOUBLE dRad = (0.5 * MIN(m_tShape.m_dHeight, m_tShape.m_dWidth)) - 1.0;
	if (dRad < 0)
	{
		dRad = 3;
	}

	al_draw_circle(dCX, dCY, dRad, tCol, 1.0);

	Turret::Draw();
}

VOID CannonTurret::_Shoot()
{
	DOUBLE dCX = m_tShape.m_dCX + m_tShape.m_dWidth/2;
	DOUBLE dCY = m_tShape.m_dCY + m_tShape.m_dHeight/2;
	DOUBLE dVX = ((m_tShape.m_dCX - m_tLastShape.m_dCX) / m_dLastLogic) / 2;
	DOUBLE dVY = ((m_tShape.m_dCY - m_tLastShape.m_dCY) / m_dLastLogic) / 2;

	switch(m_eDir)
	{
	case CDIR_DOWN:
		dVY += 300;
		break;
	case CDIR_UP:
		dVY -= 300;
		break;
	}

	CollisionManager::PhysObject::COLLISION_GROUP eGroup = (m_bPlayerOwned)?
		(CollisionManager::PhysObject::COLLISION_GROUP::GROUP_PLAYER_SHOTS):
	(CollisionManager::PhysObject::COLLISION_GROUP::GROUP_ENEMY_SHOTS);

	Cannonball *ptShot = new Cannonball(
		eGroup,
		dCX,
		dCY,
		dVX,
		dVY);

	m_ptAdder->AddWeapon(ptShot);
}