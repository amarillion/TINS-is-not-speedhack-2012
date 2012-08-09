#include <allegro5/allegro_primitives.h>

#include "starplanet.h"

StarPlanet::StarPlanet() {
	type = 0;
	aniCount = 0;
	trail = false;
	radis = 1.0;
}

void StarPlanet::init(int type, Vector2 pos) {
	int i = (type >> 1) & 0x03;

	this->pos = pos;
	this->type = type;
	// Randomize the top 5 bits of type
	if (type & 0x1) {
		color = al_map_rgb(255, 255, 255);
	}
	else {
		color = al_map_rgb(20, 255, 60);
		switch (i) {
		case 0:
			radis = 11.0;
			break;
		case 1:
			radis = 7.0;
			break;
		case 2:
			radis = 5.0;
			break;
		}
	}

}

float StarPlanet::getX() {
	return pos.x;
}

void StarPlanet::update(Vector2 speed, bool trail) {
	this->trail = trail;
	pos += speed;
}

void StarPlanet::draw() {
	if (type & 0x1) {
		// Draw the star
		al_draw_filled_circle(pos.x, pos.y, radis, color);
	}
	else {
		// Draw the planet
		al_draw_filled_circle(pos.x, pos.y, radis, color);
	}
}
