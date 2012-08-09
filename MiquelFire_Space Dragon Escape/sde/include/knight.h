#ifndef KNIGHT_H
#define KNIGHT_H

#include <allegro5/allegro.h>

#include "vector2.h"

class Knight {
public:
	Knight();
	void setBitmap(ALLEGRO_BITMAP *knight);
	void init(float startY, int width);
	void spawn();

	bool isAlive() { return state != 3; };
	bool isActive() { return (state == 1 || state == 2); }
	bool isHittable() { return state == 1; }
	bool logic();
	void draw();

	// TODO Add some padding
	Vector2 getPos() { return pos; };
	Vector2 getBox() { return Vector2(al_get_bitmap_width(image), al_get_bitmap_height(image)); };
	void hit();

private:
	Vector2 pos;
	Vector2 speed;
	Vector2 target;
	int width;
	int state;
	int aniCount;
	ALLEGRO_BITMAP *image;

};

#endif
