#include <allegro5/allegro_primitives.h>

#include "projectile.h"

Projectile::Projectile() {
	active = false;
	aniCount = 0;
}

void Projectile::init(Vector2 pos, Vector2 speed, int type, int width) {
	this->pos = pos;
	this->speed = speed;
	this->type = type;
	this->width = width;
	active = true;
}

void Projectile::logic(Dragon *dragon, Knight *knights, int knightsCount) {
	Vector2 tpos;
	Vector2 box;

	int i;

	if (!active) return;

	pos += speed;

	// Bullet moved out of bounds, kill it.
	if (pos.x < -4 || pos.x > width + 4 || pos.y < -4 || pos.y > 484) {
		active = false;
		return;
	}

	if (!dragon || !knights) {
		return;
	}

	if (type) {
		// Dragon's bullet
		for (i = 0; i < knightsCount; i++) {
			// If our box is inside the knights, call it's hit function and set active to false
			if (!knights[i].isHittable()) {
				continue;
			}
			tpos = knights[i].getPos();
			box = knights[i].getBox();
			if (
				pos.x - 2 < tpos.x + box.x - 1 &&
				pos.y - 2 < tpos.y + box.y - 1 &&
				tpos.x < this->pos.x + 2 &&
				tpos.y < this->pos.y + 2
			) {
				active = false;
				knights[i].hit();
			}
		}
	}
	else {
		// Knights's bullet
		// If our box is inside the dragon, call it's hit function and set active to false
		tpos = dragon->getPos();
		box = dragon->getBox();
		if (
			pos.x - 2 < tpos.x + box.x - 1 &&
			pos.y - 2 < tpos.y + box.y - 1 &&
			tpos.x < this->pos.x + 2 &&
			tpos.y + 10 < this->pos.y + 2
		) {
			active = false;
			dragon->hit();
		}
	}

	aniCount++;
}

void Projectile::draw() {
	if (active) {
		al_draw_filled_circle(pos.x, pos.y, 4, (type ? al_map_rgb(128 + (128 * (aniCount % 10)/10.0), 0, 0) : al_map_rgb(0, 0, 128 + (128 * (aniCount % 10)/10.0))));
	}
}
