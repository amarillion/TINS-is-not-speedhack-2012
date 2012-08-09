#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_audio.h>
#include <math.h>
#include <cstdlib>
#include <cstdio>

#include "main.h"
#include "game.h"
#include "input.h"
#include "resscale.h"
#include "starplanet.h"
#include "dragon.h"
#include "knight.h"
#include "projectile.h"

static int zoomTime = 180;

static float star0Speed = 0.5f;
static float star1Speed = 0.75f;
static float star2Speed = 1.0f;

static float planet0Speed = 0.5f;
static float planet1Speed = 0.75f;
static float planet2Speed = 1.0f;

void save_game(void) {
	ALLEGRO_PATH *path;
	ALLEGRO_FILE *progress;
	ALLEGRO_STATE state;
	char cstr[100];
	const char *file;
	int ml;

	al_store_state(&state, ALLEGRO_STATE_NEW_FILE_INTERFACE);
	al_set_standard_file_interface();
	path = al_get_standard_path(ALLEGRO_USER_SETTINGS_PATH);
	al_drop_path_tail(path);
	al_set_path_filename(path, "sdeprogress.dat");
	file = al_path_cstr(path, ALLEGRO_NATIVE_PATH_SEP);

	if (selectedLevel > maxLevel) {
		maxLevel = selectedLevel;
	}
	if (maxLevel > totalLevels) {
		maxLevel = totalLevels;
	}
	if (selectedLevel > maxLevel) {
		selectedLevel = maxLevel;
	}
	progress = al_fopen(file, "r");
	if (progress) {
		al_fgets(progress, cstr, 99);
		al_fclose(progress);
		ml = atoi(cstr);
	}
	else {
		ml = 0;
	}
	if (maxLevel > ml || ml > totalLevels) {
		ml = maxLevel;
		progress = al_fopen(file, "w");
		if (progress) {
			sprintf(cstr, "%d", maxLevel);
			al_fputs(progress, cstr);
			al_fclose(progress);
		}
	}
	if (ml > maxLevel) {
		maxLevel = ml;
	}
	if (path) al_destroy_path(path);
	al_restore_state(&state);
}

void play_game(void) {
	// Variables
	ALLEGRO_EVENT event;
	ALLEGRO_TRANSFORM def, scaled;
	ALLEGRO_FONT *hud, *status;
	ALLEGRO_COLOR white = al_map_rgb(255, 255, 255), red = al_map_rgb(255, 0, 0), yellow = al_map_rgb(255, 255, 0);
	ALLEGRO_BITMAP *dragonBmp, *knightBmp;
	ALLEGRO_SAMPLE *shot, *fire;

	StarPlanet **stars = 0;
	StarPlanet **planets = 0;
	Knight *knights = 0;
	Dragon dragon;
	// particles[10000];
	Projectile projectiles[1000];
	Vector2 scrollSpeed(-3.0, 0.0), pos, box, target;
	ResScale scale(640, 480);
	Input input;

	int levelState = -1, dragonHealth, knightsActive, knightsAlive, knightsTotal, aniCount = 0, starCount[3], planetCount[3],
		i, width = al_get_display_width(al_get_current_display()), height = al_get_display_height(al_get_current_display()), swidth = (int)(width / (float)height * 480);
	float percent, starX;
	double knightCounter = 0.0;
	bool running = true, redraw = true, fired;

	// Load files
	hud = al_load_ttf_font("VeraMoBd.ttf", -20, ALLEGRO_TTF_NO_AUTOHINT);
	status = al_load_ttf_font("VeraMoBd.ttf", -80, ALLEGRO_TTF_NO_AUTOHINT);
	dragonBmp = al_load_bitmap("dragon.png");
	if (!dragonBmp) {
		al_show_native_message_box(al_get_current_display(), "Error!", "Missing File!", "dragon.png not found", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return;
	}
	knightBmp = al_load_bitmap("knight.png");
	if (!knightBmp) {
		al_show_native_message_box(al_get_current_display(), "Error!", "Missing File!", "dragon.png not found", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		al_destroy_bitmap(dragonBmp);
		return;
	}

	shot = al_load_sample("shot.wav");
	fire = al_load_sample("fire.wav");

	dragon.setBitmap(dragonBmp);

	al_identity_transform(&def);
	scale.getTransform(&scaled);


	// create the far, middle, and near part of stars and planets
	stars = new StarPlanet *[3];
	planets = new StarPlanet *[3];
	i = 3;
	while(i--) {
		stars[i] = planets[i] = 0;
		starCount[i] = planetCount[i] = 0;
	}

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
				selectedLevel = -2; // Tell the title to return
				running = false;
				break;
			case ALLEGRO_EVENT_TIMER:
				if (input.pausePressed()) {
					// Pause menu?
					running = false;
				}
				knightsActive = 0;
				if (levelState == 3) {
					// Menu to play again or quit
					if (input.firePressed()) {
						levelState = -1;
					}
				}
				if (levelState == 2) {
					// Zooming out the stars
					percent = aniCount++ / (float)zoomTime;
					if (percent > 1.0) percent = 1.0;
					percent *= 3.0;
					percent += 1.0;

					for(i = 0; i < starCount[0]; i++) {
						stars[0][i].update(scrollSpeed * star0Speed * percent, true);
						starX = stars[0][i].getX();
						if (starX < -3.0) {
							stars[0][i].init(5,
								Vector2(
									640 + (scale.getOffsetx() * 2) + 3,
									((480 + (scale.getOffsety() * 2)) * rand() / (RAND_MAX + 1.0))
								)
							);
						}
					}
					for(i = 0; i < planetCount[0]; i++) {
						planets[0][i].update(scrollSpeed * planet0Speed * percent, true);
						starX = planets[0][i].getX();
						if (starX < -3.0) {
							planets[0][i].init(4,
								Vector2(
									640 + (scale.getOffsetx() * 2) + 3,
									((480 + (scale.getOffsety() * 2)) * rand() / (RAND_MAX + 1.0))
								)
							);
						}
					}
					for(i = 0; i < starCount[1]; i++) {
						stars[1][i].update(scrollSpeed * star1Speed * percent, true);
						starX = stars[1][i].getX();
						if (starX < -3.0) {
							stars[1][i].init(5,
								Vector2(
									640 + (scale.getOffsetx() * 2) + 3,
									((480 + (scale.getOffsety() * 2)) * rand() / (RAND_MAX + 1.0))
								)
							);
						}
					}
					for(i = 0; i < planetCount[1]; i++) {
						planets[1][i].update(scrollSpeed * planet1Speed * percent, true);
						starX = planets[1][i].getX();
						if (starX < -3.0) {
							planets[1][i].init(2,
								Vector2(
									640 + (scale.getOffsetx() * 2) + 3,
									((480 + (scale.getOffsety() * 2)) * rand() / (RAND_MAX + 1.0))
								)
							);
						}
					}
					for(i = 0; i < starCount[2]; i++) {
						stars[2][i].update(scrollSpeed * star2Speed * percent, true);
						starX = stars[2][i].getX();
						if (starX < -3.0) {
							stars[2][i].init(5,
								Vector2(
									640 + (scale.getOffsetx() * 2) + 3,
									((480 + (scale.getOffsety() * 2)) * rand() / (RAND_MAX + 1.0))
								)
							);
						}
					}
					for(i = 0; i < planetCount[2]; i++) {
						planets[2][i].update(scrollSpeed * planet2Speed * percent, true);
						starX = planets[2][i].getX();
						if (starX < -3.0) {
							planets[2][i].init(0,
								Vector2(
									640 + (scale.getOffsetx() * 2) + 3,
									((480 + (scale.getOffsety() * 2)) * rand() / (RAND_MAX + 1.0))
								)
							);
						}
					}

					dragon.logic(&input, levelState);
					for (i = 0; i < knightsTotal; i++) {
						if (knights[i].isActive()) {
							knightsActive++;
						}
						knights[i].logic();
					}
					for (i = 0; i < 1000; i++) {
						projectiles[i].logic(NULL, NULL, 0);
					}

					if (aniCount > zoomTime) {
						if (dragon.getHealth() > 0.0) {
							selectedLevel++;
						}
						save_game();
						levelState = 3;
						aniCount = 0;
					}
				}
				if (levelState == 1) {
					// Playing
					for(i = 0; i < starCount[0]; i++) {
						stars[0][i].update(scrollSpeed * star0Speed, true);
						starX = stars[0][i].getX();
						if (starX < -3.0) {
							stars[0][i].init(5,
								Vector2(
									640 + (scale.getOffsetx() * 2) + 3,
									((480 + (scale.getOffsety() * 2)) * rand() / (RAND_MAX + 1.0))
								)
							);
						}
					}
					for(i = 0; i < starCount[1]; i++) {
						stars[1][i].update(scrollSpeed * star1Speed , true);
						starX = stars[1][i].getX();
						if (starX < -3.0) {
							stars[1][i].init(5,
								Vector2(
									640 + (scale.getOffsetx() * 2) + 3,
									((480 + (scale.getOffsety() * 2)) * rand() / (RAND_MAX + 1.0))
								)
							);
						}
					}
					for(i = 0; i < starCount[2]; i++) {
						stars[2][i].update(scrollSpeed * star2Speed, true);
						starX = stars[2][i].getX();
						if (starX < -3.0) {
							stars[2][i].init(5,
								Vector2(
									640 + (scale.getOffsetx() * 2) + 3,
									((480 + (scale.getOffsety() * 2)) * rand() / (RAND_MAX + 1.0))
								)
							);
						}
					}
					for(i = 0; i < planetCount[0]; i++) {
						planets[0][i].update(scrollSpeed * planet0Speed, true);
						starX = planets[0][i].getX();
						if (starX < -3.0) {
							planets[0][i].init(4,
								Vector2(
									640 + (scale.getOffsetx() * 2) + 3,
									((480 + (scale.getOffsety() * 2)) * rand() / (RAND_MAX + 1.0))
								)
							);
						}
					}
					for(i = 0; i < planetCount[1]; i++) {
						planets[1][i].update(scrollSpeed * planet1Speed , true);
						starX = planets[1][i].getX();
						if (starX < -3.0) {
							planets[1][i].init(2,
								Vector2(
									640 + (scale.getOffsetx() * 2) + 3,
									((480 + (scale.getOffsety() * 2)) * rand() / (RAND_MAX + 1.0))
								)
							);
						}
					}
					for(i = 0; i < planetCount[2]; i++) {
						planets[2][i].update(scrollSpeed * planet2Speed, true);
						starX = planets[2][i].getX();
						if (starX < -3.0) {
							planets[2][i].init(0,
								Vector2(
									640 + (scale.getOffsetx() * 2) + 3,
									((480 + (scale.getOffsety() * 2)) * rand() / (RAND_MAX + 1.0))
								)
							);
						}
					}

					fired = dragon.logic(&input, levelState);
					if (fired) {
						// Fire dragon's 'bullet'
						pos = dragon.getPos();
						box = dragon.getBox();
						pos.x += box.x;
						pos.y += box.y/2 + 3;
						for (i = 0; i < 1000; i++) {
							if (!projectiles[i].isActive()) {
								projectiles[i].init(pos, Vector2(4.0, 0), 1, swidth);
								al_play_sample(fire, 0.5, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
								break;
							}
						}
					}

					for (i = 0; i < knightsTotal; i++) {
						if (knights[i].isActive()) {
							knightsActive++;
						}
						fired = knights[i].logic();
						if (fired) {
							// Fire knight's 'bullet'
							pos = knights[i].getPos();
							if (pos.x < swidth) {
								box = knights[i].getBox();
								pos.y += box.y/2 + 4;
								for (i = 0; i < 1000; i++) {
									if (!projectiles[i].isActive()) {
										projectiles[i].init(pos, Vector2(-4.0, 0), 0, swidth);
										al_play_sample(shot, 0.5, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
										break;
									}
								}
							}
						}
					}
					if ((al_get_time() - knightCounter) > 0.3) {
						// 5 7 11
						if (knightsActive < 11) {
							for (i = 0; i < knightsTotal; i++) {
								if (!knights[i].isActive() && knights[i].isAlive()) {
									knights[i].spawn();
									knightCounter = al_get_time();
									knightsActive++;
									break;
								}
							}
						}
					}
					for (i = 0; i < 1000; i++) {
						projectiles[i].logic(&dragon, knights, knightsTotal);
					}

					knightsAlive = 0;
					knightsActive = 0;
					for (i = 0; i < knightsTotal; i++) {
						if (knights[i].isAlive()) {
							knightsAlive++;
						}
						if (knights[i].isActive()) {
							knightsActive++;
						}
					}
					if (knightsAlive == 0) {
						levelState = 2;
					}
					if (dragon.getHealth() <= 0.0) {
						levelState = 2;
					}
				}
				if (levelState == 0) {
					// Zooming in the stars
					percent = aniCount++ / (float)zoomTime;
					percent = (1.0 - percent);
					if (percent < 0.0) percent = 0.0;
					percent *= 3.0;
					percent += 1.0;

					for(i = 0; i < starCount[0]; i++) {
						stars[0][i].update(scrollSpeed * star0Speed * percent, true);
						starX = stars[0][i].getX();
						if (starX < -3.0) {
							stars[0][i].init(5,
								Vector2(
									640 + (scale.getOffsetx() * 2) + 3,
									((480 + (scale.getOffsety() * 2)) * rand() / (RAND_MAX + 1.0))
								)
							);
						}
					}
					for(i = 0; i < starCount[1]; i++) {
						stars[1][i].update(scrollSpeed * star1Speed * percent, true);
						starX = stars[1][i].getX();
						if (starX < -3.0) {
							stars[1][i].init(5,
								Vector2(
									640 + (scale.getOffsetx() * 2) + 3,
									((480 + (scale.getOffsety() * 2)) * rand() / (RAND_MAX + 1.0))
								)
							);
						}
					}
					for(i = 0; i < starCount[2]; i++) {
						stars[2][i].update(scrollSpeed * star2Speed * percent, true);
						starX = stars[2][i].getX();
						if (starX < -3.0) {
							stars[2][i].init(5,
								Vector2(
									640 + (scale.getOffsetx() * 2) + 3,
									((480 + (scale.getOffsety() * 2)) * rand() / (RAND_MAX + 1.0))
								)
							);
						}
					}
					for(i = 0; i < planetCount[0]; i++) {
						planets[0][i].update(scrollSpeed * planet0Speed * percent, true);
						starX = planets[0][i].getX();
						if (starX < -3.0) {
							planets[0][i].init(4,
								Vector2(
									640 + (scale.getOffsetx() * 2) + 3,
									((480 + (scale.getOffsety() * 2)) * rand() / (RAND_MAX + 1.0))
								)
							);
						}
					}
					for(i = 0; i < planetCount[1]; i++) {
						planets[1][i].update(scrollSpeed * planet1Speed * percent, true);
						starX = planets[1][i].getX();
						if (starX < -3.0) {
							planets[1][i].init(2,
								Vector2(
									640 + (scale.getOffsetx() * 2) + 3,
									((480 + (scale.getOffsety() * 2)) * rand() / (RAND_MAX + 1.0))
								)
							);
						}
					}
					for(i = 0; i < planetCount[2]; i++) {
						planets[2][i].update(scrollSpeed * planet2Speed * percent, true);
						starX = planets[2][i].getX();
						if (starX < -3.0) {
							planets[2][i].init(0,
								Vector2(
									640 + (scale.getOffsetx() * 2) + 3,
									((480 + (scale.getOffsety() * 2)) * rand() / (RAND_MAX + 1.0))
								)
							);
						}
					}

					// TODO Support planets

					dragon.logic(&input, levelState);

					if (aniCount > zoomTime) {
						levelState = 1;
						aniCount = 0;
					}
				}
				if (levelState == -1) {
					// Loading the next level
					if (levels[selectedLevel].getLevel() != (selectedLevel + 1)) {
						// Some coding error occured
						return;
					}

					dragonHealth = levels[selectedLevel].getDHealth();
					dragon.init(dragonHealth, swidth);
					knightsAlive = knightsTotal = levels[selectedLevel].getKnights();
					knights = new Knight[knightsTotal];
					for (i = 0; i < knightsTotal; i++) {
						knights[i].setBitmap(knightBmp);
						knights[i].init(100 + (280 * rand() / (RAND_MAX + 1.0)), swidth);
					}
					planetCount[0] = levels[selectedLevel].getPlanets();
					starCount[0] = levels[selectedLevel].getStars();
					
					// Near layer
					starCount[2] = starCount[0] / 4;
					starCount[0] -= starCount[2];
					stars[2] = new StarPlanet[starCount[2]];
					for (i = 0; i < starCount[2]; i++) {
						stars[2][i].init(5,
							Vector2(
								((640 + (scale.getOffsetx() * 2)) * rand() / (RAND_MAX + 1.0)),
								((480 + (scale.getOffsety() * 2)) * rand() / (RAND_MAX + 1.0))
							)
						); 
					}

					planetCount[2] = planetCount[0] / 4;
					planetCount[0] -= planetCount[2];
					planets[2] = new StarPlanet[planetCount[2]];
					for (i = 0; i < planetCount[2]; i++) {
						planets[2][i].init(4,
							Vector2(
								((640 + (scale.getOffsetx() * 2)) * rand() / (RAND_MAX + 1.0)),
								((480 + (scale.getOffsety() * 2)) * rand() / (RAND_MAX + 1.0))
							)
						); 
					}

					// Middle layer
					starCount[1] = starCount[0] / 3;
					starCount[0] -= starCount[1];
					stars[1] = new StarPlanet[starCount[1]];
					for (i = 0; i < starCount[1]; i++) {
						stars[1][i].init(3,
							Vector2(
								((640 + (scale.getOffsetx() * 2)) * rand() / (RAND_MAX + 1.0)),
								((480 + (scale.getOffsety() * 2)) * rand() / (RAND_MAX + 1.0))
							)
						); 
					}

					planetCount[1] = planetCount[0] / 3;
					planetCount[0] -= planetCount[1];
					planets[1] = new StarPlanet[planetCount[1]];
					for (i = 0; i < planetCount[1]; i++) {
						planets[1][i].init(2,
							Vector2(
								((640 + (scale.getOffsetx() * 2)) * rand() / (RAND_MAX + 1.0)),
								((480 + (scale.getOffsety() * 2)) * rand() / (RAND_MAX + 1.0))
							)
						);
					}

					// Far layer
					stars[0] = new StarPlanet[starCount[0]];
					for (i = 0; i < starCount[0]; i++) {
						stars[0][i].init(1,
							Vector2(
								((640 + (scale.getOffsetx() * 2)) * rand() / (RAND_MAX + 1.0)),
								((480 + (scale.getOffsety() * 2)) * rand() / (RAND_MAX + 1.0))
							)
						);
					}

					planets[0] = new StarPlanet[planetCount[0]];
					for (i = 0; i < planetCount[0]; i++) {
						planets[0][i].init(0,
							Vector2(
								((640 + (scale.getOffsetx() * 2)) * rand() / (RAND_MAX + 1.0)),
								((480 + (scale.getOffsety() * 2)) * rand() / (RAND_MAX + 1.0))
							)
						);
					}

					knightCounter = al_get_time();
					aniCount = 0;
					levelState = 0;
					for (i = 0; i < 1000; i++) {
						projectiles[i].kill();
					}
					break;
				}

				input.resetChanged();
			case ALLEGRO_EVENT_DISPLAY_EXPOSE:
				redraw = true;
				break;
			}
		}

		if (redraw) {
			al_clear_to_color(al_map_rgb(0, 0, 0));
			if (levelState != 3) {
				al_use_transform(&scaled);
				// Draw stuff

				// Draw Stars and planets
				for (i = 0; i < starCount[0]; i++) {
					stars[0][i].draw();
				}
				for (i = 0; i < planetCount[0]; i++) {
					planets[0][i].draw();
				}
				for (i = 0; i < starCount[1]; i++) {
					stars[1][i].draw();
				}
				for (i = 0; i < planetCount[1]; i++) {
					planets[1][i].draw();
				}
				for (i = 0; i < starCount[2]; i++) {
					stars[2][i].draw();
				}
				for (i = 0; i < planetCount[2]; i++) {
					planets[2][i].draw();
				}

				// Draw particles
				// Draw knights
				for (i = 0; i < knightsTotal; i++) {
					if (knights[i].isActive()) {
						knights[i].draw();
					}
				}
				// Draw dragon
				dragon.draw();

				// Draw projectiles
				for (i = 0; i < 1000; i++) {
					projectiles[i].draw();
				}

				al_use_transform(&def);
				// Draw hud
				if (levelState != 3 && levelState != -1) {
					al_draw_filled_rectangle(80, 5, 279, 25, red); // Width is 199, which is prime ;)
					percent = dragon.getHealth() / dragonHealth;
					al_draw_filled_rectangle(80, 5, 80 + (199 * percent), 25, yellow);
					al_draw_rectangle(80, 5, 279, 25, white, 2);
					al_draw_textf(hud, white, 5, 5, 0, "Health: %05.2f", dragon.getHealth());
					al_draw_textf(hud, white, width - 5, 5, ALLEGRO_ALIGN_RIGHT, "Knights: %02d/%03d   Level: %02d/%02d", knightsActive, knightsAlive, selectedLevel + 1, totalLevels + 1);
				}
				// Draw fade box
				if (levelState == 0 || levelState == 2) {
					percent = aniCount / (float)zoomTime;
					if (percent > 1.0) percent = 1.0;
					if (levelState == 0) {
						percent = 1.0 - percent;
					}
					
					al_draw_filled_rectangle(0, 0, width, height, al_map_rgba_f(0.0, 0.0, 0.0, percent));
				}

			}
			al_use_transform(&scaled);
			// Draw status (fade it in if needed)
			if (levelState > 1) {
				if (dragon.getHealth() > 0.0) {
					al_draw_text(status, white, swidth/2, 120, ALLEGRO_ALIGN_CENTRE, "WINNER!");
				}
				else {
					al_draw_text(status, red, swidth/2, 120, ALLEGRO_ALIGN_CENTRE, "GAME OVER!");
				}
			}

			// Draw menu
			if (levelState == 3) {
				if (dragon.getHealth() > 0.0) {
					al_draw_text(hud, white, swidth/2, 200, ALLEGRO_ALIGN_CENTRE, "Press fire to play next level.");
				}
				else {
					al_draw_text(hud, red, swidth/2, 200, ALLEGRO_ALIGN_CENTRE, "Press fire to try again.");
				}
			}

			al_flip_display();
			redraw = false;
		}
	}

	// Clean up
	i = 3;
	while (i--) {
		if (stars[i]) {
			delete [] stars[i];
			stars[i] = 0;
		}
		if (planets[i]) {
			delete [] planets[i];
			planets[i] = 0;
		}
	}
	delete [] stars;
	delete [] planets;
	delete [] knights;

	al_destroy_bitmap(dragonBmp);
	al_destroy_bitmap(knightBmp);
	al_destroy_font(hud);
	al_destroy_font(status);
	al_stop_timer(timer);
	al_flush_event_queue(queue);
}
