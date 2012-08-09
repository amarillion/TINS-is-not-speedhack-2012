#include "knight.h"

Knight::Knight() {
	state = 0;
	aniCount = 0;
}

void Knight::setBitmap(ALLEGRO_BITMAP *knight) {
	image = knight;
}

void Knight::init(float startY, int width) {
	this->width = width;
	pos = Vector2(width + 10, startY);
}

void Knight::spawn() {
	state = 1;
}

bool Knight::logic() {
	Vector2 dir;

	float dist;
	bool ret = false;
	
	if (state == 3) return false;

	// Might do some randomness here. Set number in a member however
	if (state != 0) {
		if ((aniCount % 127) == 0) {
			target = Vector2(100 + ((width - 100 - al_get_bitmap_width(image)) * rand() / (RAND_MAX + 1.0)), (480 - al_get_bitmap_height(image)) * rand() / (RAND_MAX + 1.0));
		}

		// Was 157
		if ((aniCount % 67) == 0) {
			ret = true;
		}

		dir = target - pos;
		speed = dir.norm();
		pos += speed;
		dist = target.dst(pos);
		aniCount++;
	}

	return ret;
}

void Knight::draw() {
	al_draw_bitmap(image, pos.x, pos.y, 0);
}

void Knight::hit() {
	state = 3;
}
