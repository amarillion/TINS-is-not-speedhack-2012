#include "dragon.h"

Dragon::Dragon() {
}

void Dragon::setBitmap(ALLEGRO_BITMAP *dragon) {
	this->image = dragon;
}

void Dragon::init(float health, float width) {
	this->health = health;
	this->width = width;
	pos = Vector2(0.0, 240.0 - (al_get_bitmap_height(image) / 2));
}

bool Dragon::logic(Input *input, int state) {
	bool ret = false;
	int ud = 0, lr = 0;

	if (state == 0) {
		pos.x += 0.7;
		if (pos.x > 100.0) pos.x = 100.0;
	}
	if (state == 1) {
		if (input->upDown()) {
			ud -= 1;
		}
		if (input->downDown()) {
			ud += 1;
		}
		if (input->leftDown()) {
			lr -= 1;
		}
		if (input->rightDown()) {
			lr += 1;
		}

		pos.x += lr * 3.0;
		if (pos.x > width - al_get_bitmap_width(image)) {
			pos.x = width - al_get_bitmap_width(image);
		}
		if (pos.x < 0) {
			pos.x = 0;
		}
		pos.y += ud * 3.0;
		if (pos.y > 480 - al_get_bitmap_height(image)) {
			pos.y = 480 - al_get_bitmap_height(image);
		}
		if (pos.y < 0) {
			pos.y = 0;
		}

		if (input->firePressed()) {
			ret = true;
		}
	}
	if (state == 2) {
		if (health > 0.0) {
			pos.x += 3.0;
		}
		else {
		}
	}

	return ret;
}

void Dragon::draw() {
	if (health > 0.0)
		al_draw_bitmap(image, pos.x, pos.y, 0);
}

void Dragon::hit() {
	health -= 0.5;
	if (health < 0.0) {
		health = 0.0;
	}
}
