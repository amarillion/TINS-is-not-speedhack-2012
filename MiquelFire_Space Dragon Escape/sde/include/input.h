#ifndef INPUT_H
#define INPUT_H

#include <allegro5/allegro.h>

class Input {
public:
	Input();
	void keyDown(ALLEGRO_EVENT *event);
	void keyUp(ALLEGRO_EVENT *event);
	void resetChanged();

	// Getters
	bool upPressed();
	bool upDown();
	bool downPressed();
	bool downDown();
	bool leftPressed();
	bool leftDown();
	bool rightPressed();
	bool rightDown();
	bool firePressed();
	bool pausePressed();
	
private:
	int up, down, left, right, fire, pause;
};

#endif // INPUT_H