#include "SpearTurret.h"


SpearTurret::SpearTurret(IWeaponAdder *ptAdder, SDir eDir, BOOL bPlayerOwned):
Turret(ptAdder, bPlayerOwned), m_eDir(eDir)
{
	m_dLife = _MaxLife();
}

SpearTurret::~SpearTurret(void)
{
}


VOID
SpearTurret::Draw() const
{
	float fAngle = 0;

	switch(m_eDir)
	{
	case SDIR_DOWN:
		fAngle = 1.57075;
		break;
	case SDIR_UP:
		fAngle = 4.71225;
		break;
	case SDIR_LEFT:
		fAngle = 3.1415;
		break;
	case SDIR_RIGHT:
		fAngle = 0;
	}

	float fLen = MIN(m_tShape.m_dWidth, m_tShape.m_dHeight) / 2;
	float fCX = m_tShape.m_dCX + m_tShape.m_dWidth / 2;
	float fCY = m_tShape.m_dCY + m_tShape.m_dHeight / 2;

	al_draw_line(fCX, fCY, fCX + fLen*cos(fAngle), fCY + fLen*sin(fAngle), al_map_rgba(200, 200, 200, 200), 1.0);
	al_draw_line(fCX, fCY, fCX + fLen*cos(fAngle + 2.0943), fCY + fLen*sin(fAngle + 2.0943), al_map_rgba(200, 200, 200, 200), 1.0);
	al_draw_line(fCX, fCY, fCX + fLen*cos(fAngle + 4.1886), fCY + fLen*sin(fAngle + 4.1886), al_map_rgba(200, 200, 200, 200), 1.0);

	Turret::Draw();
}

VOID SpearTurret::_Shoot()
{
	DOUBLE dCX = m_tShape.m_dCX + m_tShape.m_dWidth/2;
	DOUBLE dCY = m_tShape.m_dCY + m_tShape.m_dHeight/2;
	DOUBLE dVX = ((m_tShape.m_dCX - m_tLastShape.m_dCX) / m_dLastLogic) / 2;
	DOUBLE dVY = ((m_tShape.m_dCY - m_tLastShape.m_dCY) / m_dLastLogic) / 2;

	switch(m_eDir)
	{
	case SDIR_DOWN:
		dVY += 120;
		break;
	case SDIR_UP:
		dVY -= 120;
		break;
	case SDIR_LEFT:
		dVX -= 120;
		break;
	case SDIR_RIGHT:
		dVX += 120;
		break;
	}

	CollisionManager::PhysObject::COLLISION_GROUP eGroup = (m_bPlayerOwned)?
		(CollisionManager::PhysObject::COLLISION_GROUP::GROUP_PLAYER_SHOTS):
		(CollisionManager::PhysObject::COLLISION_GROUP::GROUP_ENEMY_SHOTS);

	Harpoon *ptShot = new Harpoon(
		eGroup,
		dCX,
		dCY,
		dVX,
		dVY);

	m_ptAdder->AddWeapon(ptShot);
}

DOUBLE SpearTurret::_MaxCoolDown()
{
	float random =  (rand()%256)/256.0;
	double result = (0.5 + 0.25 * random)*((m_bPlayerOwned)?(1.0):(2.0));
	return result;
}