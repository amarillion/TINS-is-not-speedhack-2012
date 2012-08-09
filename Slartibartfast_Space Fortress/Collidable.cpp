#include "Collidable.h"

vector<CollisionManager::PhysObject *> CollisionManager::g_aObjects;
vector<CollisionManager::Collision> CollisionManager::g_aCollisions;

CollisionManager::PhysObject::PhysObject(COLLISION_GROUP eGroup, DWORD dwObjId):m_dwObjId(dwObjId),m_ptDistanceCircle(NULL),m_eCollisionGroup(eGroup)
{
}


CollisionManager::PhysObject::~PhysObject(void)
{
	DELETEPTR(m_ptDistanceCircle);
}

BOOL
Circle::CollidesWith(const Circle &tOther) const
{
	return	(m_dCX - tOther.m_dCX)*(m_dCX - tOther.m_dCX) +
		(m_dCY - tOther.m_dCY)*(m_dCY - tOther.m_dCY) <
		(m_dR - tOther.m_dR)*(m_dR - tOther.m_dR);
}

BOOL
Circle::CollidesWith(const Rect &tOther) const 
{
	return tOther.CollidesWith(*this);
}

BOOL
Rect::CollidesWith(const Circle &tOther) const
{
	if (tOther.m_dCX + tOther.m_dR < m_dCX)
	{
		return FALSE;
	}
	if (tOther.m_dCX - tOther.m_dR > m_dCX + m_dWidth)
	{
		return FALSE;
	}
	if (tOther.m_dCY + tOther.m_dR < m_dCY)
	{
		return FALSE;
	}
	if (tOther.m_dCY - tOther.m_dR > m_dCY + m_dHeight)
	{
		return FALSE;
	}

	if ((tOther.m_dCX < m_dCX) && (tOther.m_dCY < m_dCY))
	{
		if	((tOther.m_dCX - m_dCX)*(tOther.m_dCX - m_dCX)+
			(tOther.m_dCY - m_dCY)*(tOther.m_dCY - m_dCY)<
			tOther.m_dR*tOther.m_dR)
		{
			return FALSE;
		}
		return TRUE;
	}

	if ((tOther.m_dCX < m_dCX) && (tOther.m_dCY > m_dCY + m_dHeight))
	{
		if	((tOther.m_dCX - m_dCX)*(tOther.m_dCX - m_dCX)+
			(tOther.m_dCY - m_dCY - m_dHeight)*(tOther.m_dCY - m_dCY - m_dHeight)<
			tOther.m_dR*tOther.m_dR)
		{
			return FALSE;
		}
		return TRUE;
	}

	if ((tOther.m_dCX > m_dCX + m_dWidth) && (tOther.m_dCY < m_dCY))
	{
		if	((tOther.m_dCX - m_dCX - m_dWidth)*(tOther.m_dCX - m_dCX - m_dWidth)+
			(tOther.m_dCY - m_dCY)*(tOther.m_dCY - m_dCY)<
			tOther.m_dR*tOther.m_dR)
		{
			return FALSE;
		}
		return TRUE;
	}
	
	if ((tOther.m_dCX > m_dCX + m_dWidth) && (tOther.m_dCY > m_dCY + m_dHeight))
	{
		if	((tOther.m_dCX - m_dCX - m_dWidth)*(tOther.m_dCX - m_dCX - m_dWidth)+
			(tOther.m_dCY - m_dCY - m_dHeight)*(tOther.m_dCY - m_dCY - m_dHeight)<
			tOther.m_dR*tOther.m_dR)
		{
			return FALSE;
		}
		return TRUE;
	}

	return TRUE;
}
BOOL
Rect::CollidesWith(const Rect &tOther) const
{
	if (tOther.m_dCX > m_dCX + m_dWidth)
	{
		return FALSE;
	}
	if (m_dCX > tOther.m_dCX + tOther.m_dWidth)
	{
		return FALSE;
	}
	if (tOther.m_dCY > m_dCY + m_dHeight)
	{
		return FALSE;
	}
	if (m_dCY > tOther.m_dCY + tOther.m_dHeight)
	{
		return FALSE;
	}
	return TRUE;
}

VOID CollisionManager::PhysObject::MoveTo(DOUBLE dX, DOUBLE dY)
{
	if (NULL == m_ptDistanceCircle)
	{
		return;
	}

	Move(dX - m_ptDistanceCircle->m_dCX, dY - m_ptDistanceCircle->m_dCY);
}
VOID CollisionManager::PhysObject::Move(DOUBLE dX, DOUBLE dY)
{
	if (NULL == m_ptDistanceCircle)
	{
		return;
	}
	m_ptDistanceCircle->m_dCX += dX;
	m_ptDistanceCircle->m_dCY += dY;

	for (int i=0; i<m_aCircles.size(); ++i)
	{
		m_aCircles[i].m_dCX += dX;
		m_aCircles[i].m_dCY += dY;
	}

	for (int i=0 ;i<m_aRects.size(); ++i)
	{
		m_aRects[i].m_dCX += dX;
		m_aRects[i].m_dCY += dY;
	}
}

BOOL CollisionManager::PhysObject::Collides(const PhysObject &tOther, vector<Collision> &aCollisions) const
{
	BOOL bResult = FALSE;
	Collision tCollision;

	if	((GROUP_SCENERY == m_eCollisionGroup) || 
		(GROUP_SCENERY == tOther.m_eCollisionGroup) ||
		(m_eCollisionGroup == tOther.m_eCollisionGroup))
	{
		return FALSE;
	}

	if	(((GROUP_PLAYER == m_eCollisionGroup) &&
		(GROUP_PLAYER_SHOTS == tOther.m_eCollisionGroup)) ||
		((GROUP_PLAYER_SHOTS == m_eCollisionGroup) &&
		(GROUP_PLAYER == tOther.m_eCollisionGroup)))
	{
		return FALSE;
	}

	if	(((GROUP_ENEMY == m_eCollisionGroup) &&
		(GROUP_ENEMY_SHOTS == tOther.m_eCollisionGroup)) ||
		((GROUP_ENEMY_SHOTS == m_eCollisionGroup) &&
		(GROUP_ENEMY == tOther.m_eCollisionGroup)))
	{
		return FALSE;
	}

	if	(((GROUP_ENEMY == m_eCollisionGroup) &&
		(GROUP_PLAYER == tOther.m_eCollisionGroup)) ||
		((GROUP_PLAYER == m_eCollisionGroup) &&
		(GROUP_ENEMY == tOther.m_eCollisionGroup)))
	{
		return FALSE;
	}

	if ((NULL == m_ptDistanceCircle) || (NULL == tOther.m_ptDistanceCircle))
	{
		return FALSE;
	}

	if (! m_ptDistanceCircle->CollidesWith(*tOther.m_ptDistanceCircle))
	{
		return FALSE;
	}

	tCollision.dwObjId0 = m_dwObjId;
	tCollision.dwObjId1 = tOther.m_dwObjId;

	tCollision.bObj0Rect = FALSE;
	for (int i=0; i<m_aCircles.size(); ++i)
	{
		tCollision.bObj1Rect = FALSE;
		for (int j=0 ;j<tOther.m_aCircles.size(); ++j)
		{
			if (m_aCircles[i].CollidesWith(tOther.m_aCircles[j]))
			{
				tCollision.dwObj0Index = i;
				tCollision.dwObj1Index = j;
				aCollisions.push_back(tCollision);
				bResult = TRUE;
			}
		}
		tCollision.bObj1Rect = TRUE;
		for (int j=0; j<tOther.m_aRects.size(); ++j)
		{
			if (m_aCircles[i].CollidesWith(tOther.m_aRects[j]))
			{
				tCollision.dwObj0Index = i;
				tCollision.dwObj1Index = j;
				aCollisions.push_back(tCollision);
				bResult = TRUE;
			}
		}
	}

	tCollision.bObj0Rect = TRUE;
	for (int i=0; i<m_aRects.size(); ++i)
	{
		tCollision.bObj1Rect = FALSE;
		for (int j=0; j< tOther.m_aCircles.size(); ++j)
		{
			if (m_aRects[i].CollidesWith(tOther.m_aCircles[j]))
			{
				tCollision.dwObj0Index = i;
				tCollision.dwObj1Index = j;
				aCollisions.push_back(tCollision);
				bResult = TRUE;
			}
		}
		tCollision.bObj1Rect = TRUE;
		for (int j=0; j< tOther.m_aRects.size(); ++j)
		{
			if (m_aRects[i].CollidesWith(tOther.m_aRects[j]))
			{
				tCollision.dwObj0Index = i;
				tCollision.dwObj1Index = j;
				aCollisions.push_back(tCollision);
				bResult = TRUE;
			}
		}
	}

	return bResult;
}

CollisionManager::PhysObject *
CollisionManager::New(
	CollisionManager::PhysObject::COLLISION_GROUP eGroup, 
	DWORD &dwOutObjId
)
{
	DWORD dwObjId = g_aObjects.size();
	g_aObjects.push_back(new CollisionManager::PhysObject(eGroup, dwObjId));
	dwOutObjId = dwObjId;
	return g_aObjects[dwObjId];
}

BOOL
CollisionManager::Delete(
	DWORD dwObjId
)
{
	if (NULL != g_aObjects[dwObjId])
	{
		delete g_aObjects[dwObjId];
		g_aObjects[dwObjId] = NULL;
		return TRUE;
	}
	return FALSE;
}

VOID
CollisionManager::CalculateCollisions()
{
	g_aCollisions.clear();

	for (int i=0 ; i<g_aObjects.size(); ++i)
	{
		if (NULL == g_aObjects[i])
		{
			continue;
		}
		for (int j=i+1; j<g_aObjects.size(); ++j)
		{
			if (NULL == g_aObjects[j])
			{
				continue;
			}
			g_aObjects[i]->Collides(*g_aObjects[j], g_aCollisions);
		}
	}
}

const vector<CollisionManager::Collision> &
CollisionManager::GetCollisions()
{
	return g_aCollisions;
}