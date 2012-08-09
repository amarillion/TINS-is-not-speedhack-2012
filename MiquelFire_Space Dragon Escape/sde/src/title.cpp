#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <cstdio>

#include "main.h"
#include "title.h"
#include "input.h"
#include "resscale.h"

bool title_screen(void) {
	if (selectedLevel == -2) {
		return false;
	}
	ALLEGRO_FONT *fontTitle, *fontMenu;
	ALLEGRO_TRANSFORM scale;
	ALLEGRO_EVENT event;
	ALLEGRO_COLOR black = al_map_rgb(0, 0, 0), red = al_map_rgb(255, 0, 0), white = al_map_rgb(255, 255, 255), offWhite = al_map_rgba(255, 255, 255, 128);

	Input input;
	ResScale scaler(640, 480);

	bool running = true, ret = false, redraw = true;
	int selection = 0, x, y, w, h;
	float ngy, cgy, qy;
	char cstr[100];

	scaler.getTransform(&scale);
	al_use_transform(&scale);

	fontTitle = al_load_ttf_font("VeraMoBd.ttf", -50, ALLEGRO_TTF_NO_AUTOHINT);
	if (!fontTitle) return false;
	fontMenu = al_load_ttf_font("VeraMoBd.ttf", -20, ALLEGRO_TTF_NO_AUTOHINT);
	if (!fontMenu) {
		al_destroy_font(fontTitle);
		return false;
	}

	al_get_text_dimensions(fontMenu, "New Game", &x, &y, &w, &h);
	ngy = 300 + scaler.getOffsety();
	cgy = ngy + h + 5;
	qy = cgy + h + 5;

	al_start_timer(timer);
	while (running) {
		al_wait_for_event(queue, NULL);
		while (!al_event_queue_is_empty(queue)) {
			al_get_next_event(queue, &event);

			switch (event.type) {
			case ALLEGRO_EVENT_KEY_DOWN:
				input.keyDown(&event);
				break;
			case ALLEGRO_EVENT_KEY_UP:
				input.keyUp(&event);
				break;
			case ALLEGRO_EVENT_DISPLAY_CLOSE:
				running = false;
				break;
			case ALLEGRO_EVENT_TIMER:
				if (input.pausePressed()) {
					running = false;
				}
				if (input.firePressed()) {
					running = false;
					ret = !((maxLevel && selection == 2) || (!maxLevel && selection == 1));
				}

				if (input.upPressed()) {
					selection--;
					if (selection < 0) selection = maxLevel ? 2 : 1;
				}
				if (input.downPressed()) {
					selection = (selection + 1) % (maxLevel ? 3 : 2);
				}
				if (maxLevel && selection == 1) {
					if (input.leftPressed()) {
						selectedLevel--;
						if (selectedLevel < 0) selectedLevel = maxLevel;
					}

					if (input.rightPressed()) {
						selectedLevel = (selectedLevel + 1) % (maxLevel + 1);
					}
				}

				input.resetChanged();
			case ALLEGRO_EVENT_DISPLAY_EXPOSE:
				redraw = true;
				break;
			}
		}

		if (redraw) {
			al_use_transform(&scale);
			al_clear_to_color(black);

			al_draw_text(fontTitle, red, 320 + scaler.getOffsetx(), 80 + scaler.getOffsety(), ALLEGRO_ALIGN_CENTRE, "Space Dragon Escape");

			switch (selection) {
			case 0:
				if (maxLevel) {
					sprintf(cstr, "Select Level: %02d", selectedLevel + 1);
					al_draw_text(fontMenu, white, 320 + scaler.getOffsetx(), ngy, ALLEGRO_ALIGN_CENTRE, "> Continue <");
					al_draw_text(fontMenu, offWhite, 320 + scaler.getOffsetx(), cgy, ALLEGRO_ALIGN_CENTRE, cstr);
					al_draw_text(fontMenu, offWhite, 320 + scaler.getOffsetx(), qy, ALLEGRO_ALIGN_CENTRE, "Quit");
				}
				else {
					al_draw_text(fontMenu, white, 320 + scaler.getOffsetx(), ngy, ALLEGRO_ALIGN_CENTRE, "> New Game <");
					al_draw_text(fontMenu, offWhite, 320 + scaler.getOffsetx(), cgy, ALLEGRO_ALIGN_CENTRE, "Quit");
				}
				break;
			case 1:
				if (maxLevel) {
					sprintf(cstr, "> Select Level: %02d <", selectedLevel + 1);
					al_draw_text(fontMenu, offWhite, 320 + scaler.getOffsetx(), ngy, ALLEGRO_ALIGN_CENTRE, "Continue");
					al_draw_text(fontMenu, white, 320 + scaler.getOffsetx(), cgy, ALLEGRO_ALIGN_CENTRE, cstr);
					al_draw_text(fontMenu, offWhite, 320 + scaler.getOffsetx(), qy, ALLEGRO_ALIGN_CENTRE, "Quit");
				}
				else {
					al_draw_text(fontMenu, offWhite, 320 + scaler.getOffsetx(), ngy, ALLEGRO_ALIGN_CENTRE, "New Game");
					al_draw_text(fontMenu, white, 320 + scaler.getOffsetx(), cgy, ALLEGRO_ALIGN_CENTRE, "> Quit <");
				}
				break;
			case 2:
				sprintf(cstr, "Select Level: %02d", selectedLevel + 1);
				al_draw_text(fontMenu, offWhite, 320 + scaler.getOffsetx(), ngy, ALLEGRO_ALIGN_CENTRE, "Continue");
				al_draw_text(fontMenu, offWhite, 320 + scaler.getOffsetx(), cgy, ALLEGRO_ALIGN_CENTRE, cstr);
				al_draw_text(fontMenu, white, 320 + scaler.getOffsetx(), qy, ALLEGRO_ALIGN_CENTRE, "> Quit <");
				break;
			}

			al_flip_display();
			redraw = false;
		}
	}
	al_stop_timer(timer);

	al_destroy_font(fontTitle);
	al_destroy_font(fontMenu);
	return ret;
}
