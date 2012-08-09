#ifndef DRAGON_H
#define DRAGON_H

#include <allegro5/allegro.h>

#include "vector2.h"
#include "input.h"

class Dragon {
public:
	Dragon();
	void setBitmap(ALLEGRO_BITMAP *dragon);
	void init(float health, float width);

	bool logic(Input *input, int state);
	void draw();

	// TODO Add some padding
	Vector2 getPos() { return pos; };
	Vector2 getBox() { return Vector2(al_get_bitmap_width(image), al_get_bitmap_height(image)); };
	void hit();
	float getHealth() { return health; };

private:
	float health;
	float width;
	Vector2 pos;
	int aniCount;
	ALLEGRO_BITMAP *image;

};

#endif
