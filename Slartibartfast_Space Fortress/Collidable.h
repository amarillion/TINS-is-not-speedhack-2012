#ifndef Collidable_h__
#define Collidable_h__

#include "Common.h"

class Rect;

class Circle
{
public:
	Circle(){}
	virtual~Circle(){}

	BOOL CollidesWith(const Circle &tOther) const;
	BOOL CollidesWith(const Rect &tOther) const;

	DOUBLE m_dCX,m_dCY;
	DOUBLE m_dR;
};

class Rect
{
public:
	Rect(){}
	virtual~Rect(){}

	BOOL CollidesWith(const Circle &tOther) const;
	BOOL CollidesWith(const Rect &tOther) const;

	DOUBLE m_dCX, m_dCY;
	DOUBLE m_dWidth, m_dHeight;
};

class CollisionManager
{
public:
	struct Collision
	{
		DWORD dwObjId0;
		DWORD dwObjId1;
		BOOL bObj0Rect;
		BOOL bObj1Rect;
		DWORD dwObj0Index;
		DWORD dwObj1Index;
	};

	class PhysObject
	{
	public:
		typedef enum COLLISION_GROUP
		{
			GROUP_SCENERY,
			GROUP_PLAYER,
			GROUP_PLAYER_SHOTS,
			GROUP_ENEMY,
			GROUP_ENEMY_SHOTS
		};

		PhysObject(COLLISION_GROUP eGroup, DWORD dwObjId);
		virtual~PhysObject(void);

		Circle *m_ptDistanceCircle;
		vector<Circle> m_aCircles;
		vector<Rect> m_aRects;
		COLLISION_GROUP m_eCollisionGroup;
		const DWORD m_dwObjId;

		VOID MoveTo(DOUBLE dX, DOUBLE dY);
		VOID Move(DOUBLE dX, DOUBLE dY);

		BOOL Collides(const PhysObject &tOther, vector<Collision> &aCollisions) const;
	};

	// Ownership of the object belongs to CollisionManager. free by calling
	// Delete on the given object id
	static
	PhysObject *
	New(
		CollisionManager::PhysObject::COLLISION_GROUP eGroup, 
		DWORD &dwOutObjId
	);

	static
	BOOL
	Delete(
		DWORD dwObjId
	);

	static
	VOID
	CalculateCollisions();

	static
	const vector<Collision> &
	GetCollisions();

private:
	static vector<PhysObject *> g_aObjects;
	static vector<Collision> g_aCollisions;
};

#endif // Collidable_h__