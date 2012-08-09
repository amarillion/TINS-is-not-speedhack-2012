#include "Turret.h"


Turret::Turret(IWeaponAdder *ptAdder, BOOL bPlayerOwned)
{
	memset(this, 0, sizeof(*this));
	m_ptAdder = ptAdder;
	m_bPlayerOwned = bPlayerOwned;
}


Turret::~Turret(void)
{
}

VOID 
Turret::_DrawRect(
	const Rect &tShape, 
	DOUBLE dAlphaMod
)const
{
	ALLEGRO_COLOR tCol = al_map_rgba(200, 200 * dAlphaMod, 55 * dAlphaMod, 255 * dAlphaMod);
	for (float fWidth = 2.0; fWidth > 0.5; fWidth-=1.0)
	{
		al_draw_line(	tShape.m_dCX, 
						tShape.m_dCY, 
						tShape.m_dCX + tShape.m_dWidth, 
						tShape.m_dCY, 
						tCol,
						fWidth);
		al_draw_line(	tShape.m_dCX, 
						tShape.m_dCY + tShape.m_dHeight, 
						tShape.m_dCX + tShape.m_dWidth, 
						tShape.m_dCY + tShape.m_dHeight, 
						tCol,
						fWidth);
		al_draw_line(	tShape.m_dCX, 
						tShape.m_dCY, 
						tShape.m_dCX, 
						tShape.m_dCY + tShape.m_dHeight, 
						tCol,
						fWidth);
		al_draw_line(	tShape.m_dCX + tShape.m_dWidth, 
						tShape.m_dCY, 
						tShape.m_dCX + tShape.m_dWidth, 
						tShape.m_dCY + tShape.m_dHeight, 
						tCol,
						fWidth);
	}
}

VOID
Turret::Draw() const
{
	_DrawRect(m_tShape,  0.6*m_dLife / _MaxLife());
	_DrawRect(m_tLastShape,  0.4*m_dLife / _MaxLife());
}

VOID
Turret::Move(
	const Rect &tNewPlace
)
{
	m_tShape = tNewPlace;
}

VOID
Turret::Shoot()
{
	if (m_dCoolDown <= 0)
	{
		_Shoot();
		m_dCoolDown = _MaxCoolDown();
	}
}

VOID
Turret::Logic(DOUBLE dTimeDelta)
{
	if (m_dCoolDown > dTimeDelta)
	{
		m_dCoolDown -= dTimeDelta;
	}
	else
	{
		m_dCoolDown = -0.001;
	}
	m_tLastShape = m_tShape;
	m_dLastLogic = dTimeDelta;
}

BOOL
Turret::IsAlive() const
{
	return (m_dLife > 0);
}

VOID
Turret::Hit(
	DOUBLE dDamage, 
	DOUBLE dPenetration
)
{
	DOUBLE dReduction;
	if (dDamage < 0)
	{
		dReduction = dDamage;
	}
	else
	{
		dReduction = MAX((dDamage - dPenetration * _Shield()), 0);
	}

	m_dLife -= dReduction;

	if (!IsAlive())
	{
		return ;
	}
	
	if (m_dLife > _MaxLife())
	{
		m_dLife = _MaxLife();
	}
}