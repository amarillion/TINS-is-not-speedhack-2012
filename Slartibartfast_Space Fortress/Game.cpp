#include "Game.h"


Game::Game(void)
{
	m_ptPlayer = NULL;
}


Game::~Game(void)
{
	DELETEPTR(m_ptPlayer);
}

DWORD Game::_GetNextPrime(DWORD dwPrime)
{
	DWORD dwNextPrime = dwPrime + 1;
	for(;!_IsPrime(dwNextPrime);++dwNextPrime);
	return dwNextPrime;
}

VOID Game::AddWeapon(Weapon *ptShot)
{
	m_aShots[ptShot->GetPhysObj()->m_dwObjId] = ptShot;
}

VOID Game::_Logic(DOUBLE dTimeDelta)
{
	for (map<DWORD, Weapon *>::iterator iter = m_aShots.begin();
		iter != m_aShots.end();
		++iter)
	{
		iter->second->Logic(dTimeDelta);
	}

	for (map<DWORD, Dragonship *>::iterator iter = m_aEnemies.begin();
		iter != m_aEnemies.end();
		++iter)
	{
		iter->second->Logic(dTimeDelta);
	}

	m_ptPlayer->Logic(dTimeDelta);

	_ResolveCollisions();

	_CullDead();

	if (0 == m_aEnemies.size())
	{
		_LoadLevel(_GetNextPrime(m_dwCurrentLevel));
	}
}

VOID Game::_GenerateBG()
{
	m_ptSceneryBG0 = al_create_bitmap(1024, 768);
	m_ptSceneryBG1 = al_create_bitmap(1024, 768);

	const DWORD dwFactor = 16;

	al_set_target_bitmap(m_ptSceneryBG0);
	al_clear_to_color(al_map_rgb(0,0,0));
	for (DWORD i=0; i<1024/dwFactor; ++i)
	{
		for (DWORD j=0; j<768/dwFactor; ++j)
		{
			if (_IsPrime((i*dwFactor + 1)*768 + j*dwFactor + 1))
			{
				float fRadius = 1.0+rand()%3;
				unsigned char r = 128 + rand()%128;
				unsigned char g = 64 + rand()%128;
				unsigned char b = rand()%128;
				unsigned char a = 50 + rand()%50;
				float xoff = rand()%dwFactor;
				float yoff = rand()%dwFactor;
				al_draw_filled_circle(i*dwFactor + xoff, j*dwFactor + yoff, fRadius, al_map_rgba(r, g, b, a));
			}
		}
	}
	al_flip_display();

	al_set_target_bitmap(m_ptSceneryBG1);
	al_clear_to_color(al_map_rgba(0,0,0,0));
	for (DWORD i=0; i<1024/dwFactor; ++i)
	{
		for (DWORD j=0; j<768/dwFactor; ++j)
		{
			if (_IsPrime(((i*dwFactor + 1)*768 + j*dwFactor + 1)*1.5))
			{
				float fRadius = 1.0+rand()%2;
				unsigned char r = 128 + rand()%128;
				unsigned char g = 64 + rand()%128;
				unsigned char b = rand()%128;
				unsigned char a = rand()%50;
				float xoff = rand()%dwFactor;
				float yoff = rand()%dwFactor;
				al_draw_filled_circle(i*dwFactor + xoff, j*dwFactor + yoff, fRadius, al_map_rgba(r, g, b, a));
			}
		}
	}
	al_flip_display();
}

VOID Game::_Draw()
{
	float f0Height = m_dGameLen * 30;
	float f1Height = m_dGameLen * 30 / 4;

	while (f0Height > 0)
	{
		f0Height -= 768;
	}
	while (f1Height > 0)
	{
		f1Height -= 768;
	}

	al_draw_bitmap(m_ptSceneryBG0, 0, f0Height, 0);
	al_draw_bitmap(m_ptSceneryBG0, 0, f0Height + 768, 0);
	al_draw_bitmap(m_ptSceneryBG1, 0, f1Height, 0);
	al_draw_bitmap(m_ptSceneryBG1, 0, f1Height + 768, 0);

	for (map<DWORD, Weapon *>::iterator iter = m_aShots.begin();
		iter != m_aShots.end();
		++iter)
	{
		iter->second->Draw();
	}

	for (map<DWORD, Dragonship *>::iterator iter = m_aEnemies.begin();
		iter != m_aEnemies.end();
		++iter)
	{
		iter->second->Draw();
	}

	m_ptPlayer->Draw();
}

VOID Game::_ResolveCollisions()
{
	CollisionManager::CalculateCollisions();

	const vector<CollisionManager::Collision> &aCollisions = CollisionManager::GetCollisions();

	for(int i=0; i < aCollisions.size(); ++i)
	{
		map<DWORD, Weapon *>::iterator iShot;
		map<DWORD, Dragonship *>::iterator iEnemy;

		iShot = m_aShots.find(aCollisions[i].dwObjId0);
		if (iShot == m_aShots.end())
		{
			iShot = m_aShots.find(aCollisions[i].dwObjId1);
			iEnemy = m_aEnemies.find(aCollisions[i].dwObjId0);
		}
		else
		{
			iEnemy = m_aEnemies.find(aCollisions[i].dwObjId1);
		}

		if (iShot == m_aShots.end())
		{
			continue;
		}

		if	(((iEnemy == m_aEnemies.end()) && 
			(m_ptPlayer->GetObjId() != aCollisions[i].dwObjId0)) && 
			(m_ptPlayer->GetObjId() != aCollisions[i].dwObjId1))
		{
			continue;
		}

		if (iEnemy == m_aEnemies.end())
		{
			m_ptPlayer->Hit(aCollisions[i], iShot->second->GetDamage(), iShot->second->GetPenetration());
		}
		else
		{
			iEnemy->second->Hit(aCollisions[i], iShot->second->GetDamage(), iShot->second->GetPenetration());
		}
		iShot->second->Kill();
	}
}

VOID Game::_CullDead()
{
	BOOL bFinished = FALSE;

	while (!bFinished)
	{
		bFinished = TRUE;
		for (map<DWORD, Weapon *>::iterator iter = m_aShots.begin();
			iter != m_aShots.end();
			++iter)
		{
			if (!iter->second->IsAlive())
			{
				delete iter->second;
				m_aShots.erase(iter);
				bFinished = FALSE;
				break;
			}
		}
	}
	
	bFinished = FALSE;
	while (!bFinished)
	{
		bFinished = TRUE;
		for (map<DWORD, Dragonship *>::iterator iter = m_aEnemies.begin();
			iter != m_aEnemies.end();
			++iter)
		{
			if (!iter->second->IsAlive())
			{
				delete iter->second;
				m_aEnemies.erase(iter);
				bFinished = FALSE;
				break;
			}
		}
	}
}

BOOL Game::_IsPrime(DWORD dwNum)
{
	if (0 == dwNum)
	{
		return FALSE;
	}
	if (2 == dwNum)
	{
		return TRUE;
	}
	for (DWORD dwDivide = 2; dwDivide < sqrt((float)dwNum) + 1; ++dwDivide)
	{
		if (0 == dwNum % dwDivide)
		{
			return FALSE;
		}
	}
	return TRUE;
}

VOID Game::_GetNextLocation(int &iX, int &iY)
{
	int iDis = MAX(ABS(iX), ABS(iY));
	if (0 == iDis)
	{
		iX = iDis - 1;
		iX = iDis - 1;
	}
	else
	{
		if ((ABS(iY) != iDis) && (iX == -iDis))
		{
			iX = iDis;
		}
		else
		{
			iX++;
			if(iX > iDis)
			{
				iX = -iDis;
				iY++;
			}
		}
		if ((-iDis == iX) && (iDis+1 == iY))
		{
			iX = -iDis - 1;
			iY = -iDis - 1;
		}
	}
}

Game::TurretType Game::_GetType(DWORD dwIndex)
{
	if (3 > dwIndex)
	{
		return Game::TurretType::TYPE_HARPOON;
	}
	if (7 > dwIndex)
	{
		return Game::TurretType::TYPE_CANNON;
	}
	if (11 > dwIndex)
	{
		return Game::TurretType::TYPE_SHIELD;
	}
	float fRand = (rand()%1024) / 1024.0;
	if (fRand < 0.33)
	{
		return Game::TurretType::TYPE_SHIELD; 
	}
	if (fRand < 0.5)
	{
		return Game::TurretType::TYPE_CANNON;
	}
	return Game::TurretType::TYPE_HARPOON;
}

VOID Game::_AddShip(BOOL bPlayer, DWORD dwStrength)
{
	SpaceShip *ptShip = NULL;
	if (bPlayer)
	{
		ptShip = new PlayerShip();
	}
	else
	{
		ptShip = new Dragonship();
	}

	int iX = 0;
	int iY = 0;
	for (DWORD dwIndex = 0; dwIndex < dwStrength; ++dwIndex)
	{
		CannonTurret::CDir cDir = (bPlayer)?(CannonTurret::CDir::CDIR_UP):(CannonTurret::CDir::CDIR_DOWN);
		SpearTurret::SDir sDir = (bPlayer)?(SpearTurret::SDir::SDIR_UP):(SpearTurret::SDir::SDIR_DOWN);
		switch(_GetType(dwIndex))
		{
		case TYPE_CANNON:
			ptShip->_AddTurret(new CannonTurret(this, cDir, bPlayer), iX, iY);
			break;
		case TYPE_HARPOON:
			ptShip->_AddTurret(new SpearTurret(this, sDir, bPlayer), iX, iY);
			break;
		case TYPE_SHIELD:
			ptShip->_AddTurret(new DefenceTurret(this, bPlayer), iX, iY);
			break;
		}
		_GetNextLocation(iX, iY);
	}

	if (bPlayer)
	{
		DELETEPTR(m_ptPlayer);
		m_ptPlayer = dynamic_cast<PlayerShip *>(ptShip);
	}
	else
	{
		m_aEnemies[ptShip->GetObjId()] = dynamic_cast<Dragonship *>(ptShip);
	}

}

VOID Game::_LoadLevel(DWORD dwLevel)
{
	if (!_IsPrime(dwLevel))
	{
		return;
	}

	m_aEnemies.clear();
	m_aShots.clear();

	for (DWORD dwIndex = 0; dwIndex < dwLevel; ++dwIndex)
	{
		_AddShip(FALSE, dwIndex+1);
	}

	_AddShip(TRUE, dwLevel);

	m_dwCurrentLevel = dwLevel;
}

VOID Game::Start(DWORD dwStartingLevel)
{
	_LoadLevel(dwStartingLevel);
	_StartLoop();
}

VOID Game::_StartLoop()
{
	if (!al_init())
	{
		return;
	}
	if (!al_install_keyboard())
	{
		return;
	}
	if (!al_init_primitives_addon())
	{
		return;
	}

	al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
	al_set_new_display_option(ALLEGRO_SAMPLES, 4, ALLEGRO_SUGGEST);

	ALLEGRO_DISPLAY *ptDisplay = al_create_display(1024, 768);

	ALLEGRO_TIMER *ptTimer = al_create_timer(1.0/60.0);
	ALLEGRO_EVENT_QUEUE *ptQueue = al_create_event_queue();

	al_register_event_source(ptQueue, al_get_timer_event_source(ptTimer));
	al_register_event_source(ptQueue, al_get_keyboard_event_source());

	_GenerateBG();
	al_set_target_backbuffer(ptDisplay);

	al_start_timer(ptTimer);

	BOOL bExit = FALSE;
	ALLEGRO_EVENT tEvent;
	DOUBLE dLastTime = -1;
	m_dGameLen = 0;
	
	while (!bExit)
	{
		al_wait_for_event(ptQueue, &tEvent);
		switch (tEvent.type)
		{
		case ALLEGRO_EVENT_KEY_DOWN:
			if (ALLEGRO_KEY_ESCAPE == tEvent.keyboard.keycode)
			{
				bExit = TRUE;
			}
		case ALLEGRO_EVENT_KEY_UP:
			m_ptPlayer->KeyEvent(tEvent);
			break;

		case ALLEGRO_EVENT_TIMER:

			if (al_get_time() - tEvent.timer.timestamp < 0.05)
			{
				al_clear_to_color(al_map_rgb(0,0,0));
				_Draw();
				al_flip_display();

				if (dLastTime > 0)
				{
					_Logic(tEvent.timer.timestamp - dLastTime);
					m_dGameLen += tEvent.timer.timestamp - dLastTime;
				}
				dLastTime = tEvent.timer.timestamp;
			}

			break;
		}

		
	}
}