#include "input.h"

Input::Input() :
up(0), down(0), left(0), right(0), fire(0), pause(0)
{
}

void Input::keyDown(ALLEGRO_EVENT *event) {
	if (event->type != ALLEGRO_EVENT_KEY_DOWN) return;
	switch (event->keyboard.keycode) {
	case ALLEGRO_KEY_W:
	case ALLEGRO_KEY_UP:
		up = 0x03;
		break;
	case ALLEGRO_KEY_S:
	case ALLEGRO_KEY_DOWN:
		down = 0x03;
		break;
	case ALLEGRO_KEY_A:
	case ALLEGRO_KEY_LEFT:
		left = 0x03;
		break;
	case ALLEGRO_KEY_D:
	case ALLEGRO_KEY_RIGHT:
		right = 0x03;
		break;
	case ALLEGRO_KEY_SPACE:
	case ALLEGRO_KEY_ENTER:
		fire = 0x03;
		break;
	case ALLEGRO_KEY_ESCAPE:
		pause = 0x03;
		break;
	}
}

void Input::keyUp(ALLEGRO_EVENT *event) {
	if (event->type != ALLEGRO_EVENT_KEY_UP) return;
	switch (event->keyboard.keycode) {
	case ALLEGRO_KEY_W:
	case ALLEGRO_KEY_UP:
		up = up & ~0x01;
		break;
	case ALLEGRO_KEY_S:
	case ALLEGRO_KEY_DOWN:
		down = down & ~0x01;
		break;
	case ALLEGRO_KEY_A:
	case ALLEGRO_KEY_LEFT:
		left = left & ~0x01;
		break;
	case ALLEGRO_KEY_D:
	case ALLEGRO_KEY_RIGHT:
		right = right & ~0x01;
		break;
	case ALLEGRO_KEY_SPACE:
	case ALLEGRO_KEY_ENTER:
		fire = fire & ~0x01;
		break;
	case ALLEGRO_KEY_ESCAPE:
		pause = pause & ~0x01;
		break;
	}
}

void Input::resetChanged() {
	up = up & ~0x02;
	down = down & ~0x02;
	left = left & ~0x02;
	right = right & ~0x02;
	fire = fire & ~0x02;
	pause = pause & ~0x02;
}

bool Input::upPressed() {
	return (bool)(up & 0x02);
}

bool Input::upDown() {
	return (bool)(up & 0x01);
}

bool Input::downPressed() {
	return (bool)(down & 0x02);
}

bool Input::downDown() {
	return (bool)(down & 0x01);
}

bool Input::leftPressed() {
	return (bool)(left & 0x02);
}

bool Input::leftDown() {
	return (bool)(left & 0x01);
}

bool Input::rightPressed() {
	return (bool)(right & 0x02);
}

bool Input::rightDown() {
	return (bool)(right & 0x01);
}
	
bool Input::firePressed() {
	return (bool)(fire & 0x02);
}
	
bool Input::pausePressed() {
	return (bool)(pause & 0x02);
}
