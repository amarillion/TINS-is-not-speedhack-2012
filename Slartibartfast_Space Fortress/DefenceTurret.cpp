#include "DefenceTurret.h"


DefenceTurret::DefenceTurret(IWeaponAdder *ptAdder, BOOL bPlayerOwned):Turret(ptAdder, bPlayerOwned)
{
	m_dLife = _MaxLife();
}


DefenceTurret::~DefenceTurret(void)
{
}

VOID
DefenceTurret::Draw() const
{
	ALLEGRO_COLOR tCol = al_map_rgba(100, 255, 100, 200 * m_dLife / _MaxLife());
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