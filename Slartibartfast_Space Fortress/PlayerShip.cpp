#include "PlayerShip.h"


PlayerShip::PlayerShip(void):SpaceShip(CollisionManager::PhysObject::COLLISION_GROUP::GROUP_PLAYER)
{
}


PlayerShip::~PlayerShip(void)
{
}

VOID PlayerShip::KeyEvent(const ALLEGRO_EVENT &tEvent)
{
	BOOL bFlag = FALSE;
	switch(tEvent.type)
	{
	case ALLEGRO_EVENT_KEY_DOWN:
		bFlag = TRUE;
		break;
	case ALLEGRO_EVENT_KEY_UP:
		bFlag = FALSE;
		break;
	default:
		return;
	}

	switch(tEvent.keyboard.keycode)
	{
	case ALLEGRO_KEY_W:
	case ALLEGRO_KEY_UP:
		m_bUp = bFlag;
		break;
	case ALLEGRO_KEY_A:
	case ALLEGRO_KEY_LEFT:
		m_bLeft = bFlag;
		break;
	case ALLEGRO_KEY_S:
	case ALLEGRO_KEY_DOWN:
		m_bDown = bFlag;
		break;
	case ALLEGRO_KEY_D:
	case ALLEGRO_KEY_RIGHT:
		m_bRight = bFlag;
		break;
	case ALLEGRO_KEY_J:
	case ALLEGRO_KEY_SPACE:
		m_bShoot = bFlag;
		break;
	}
}