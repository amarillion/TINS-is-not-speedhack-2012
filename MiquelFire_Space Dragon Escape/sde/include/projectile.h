#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "vector2.h"
#include "dragon.h"
#include "knight.h"

class Projectile {
public:
	Projectile();
	void init(Vector2 pos, Vector2 speed, int type, int width);

	void logic(Dragon *dragon, Knight *knights, int knightsCount);
	bool isActive() { return active; }
	void kill() { active = false; }
	void draw();

private:
	Vector2 pos;
	Vector2 speed;
	int type;
	int width;
	bool active;
	int aniCount;

};

#endif
