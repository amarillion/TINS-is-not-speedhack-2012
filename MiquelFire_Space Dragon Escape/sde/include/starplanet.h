#ifndef STARPLANET_H
#define STARPLANET_H

#include "vector2.h"

class StarPlanet {
public:
	StarPlanet();

	void init(int type, Vector2 pos);
	float getX();
	void update(Vector2 speed, bool trail);
	void draw();

private:
	Vector2 pos;
	int type;
	int aniCount;
	bool trail;
	float radis;
	ALLEGRO_COLOR color;

};

#endif
