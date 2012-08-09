#include "main.h"
#define _scramble_min(x, y) ((y) < (x) ? (y) : (x))
char * main_data_path;
LandFont * font;
static char * save_path;
double scale = 2;
Application * app;
void restart(void) {
    actors_reset(app->actors);
    wave_restart();
}
#define VAL(x) land_file_write(f, (void * ) & x, sizeof(x))
void save(void) {
    LandFile * f = land_file_new(save_path, "wb");
    int n = land_array_count(app->actors->array);
    VAL(wave_ticks);
    VAL(n);
    {LandArrayIterator __iter0__ = LandArrayIterator_first(app->actors->array) ; for (Actor * a = LandArrayIterator_item(app->actors->array, & __iter0__) ; LandArrayIterator_next(app->actors->array, & __iter0__) ; a = LandArrayIterator_item(app->actors->array, & __iter0__)) {
        VAL( * a);
        VAL(a->kind->id);
        int tid = 0;
        if (a->target) {
            tid = a->target->id;
        }
        VAL(tid);
    }
    }
    for (int y = 0; y < app->actors->h; y += 1) {
        for (int x = 0; x < app->actors->w; x += 1) {
            Item * i = app->actors->tilemap[x + app->actors->w * y] .item;
            if (i) {
                VAL( * i);
            }
        }
    }
    n = 0;
    VAL(n);
    land_file_destroy(f);
}
#undef VAL
#define VAL(x) land_file_read(f, (void * ) & x, sizeof(x))
void load(void) {
    LandFile * f = land_file_new(save_path, "rb");
    int n;
    actors_reset(app->actors);
    VAL(wave_ticks);
    VAL(n);
    Actor * a;
    for (int i = 0; i < n; i += 1) {
        land_alloc(a);
        land_array_add(app->actors->array, a);
        actor_place(a);
    }
    for (int i = 0; i < n; i += 1) {
        a = land_array_get_nth(app->actors->array, i);
        VAL( * a);
        int kid, tid;
        VAL(kid);
        VAL(tid);
        a->kind = actor_type[kid];
        if (tid) {
            a->target = land_array_get_nth(app->actors->array, tid - 1);
        }
    }
    while (1) {
        VAL(n);
        if (! n) {
            break;
        }
        Item * it;
        land_alloc(it);
        it->kind = n;
        VAL(it->x);
        VAL(it->y);
        VAL(it->angle);
        item_place(it);
    }
    land_file_destroy(f);
}
#undef VAL
void runner_init(LandRunner * self) {
    land_clear(0, 0, 1, 1);
    land_flip();
    land_display_title("raytracing, please wait");
    save_path = land_get_save_file("QPD", "save");
    font = font_embed();
    land_alloc(app);
    app->w = 1280;
    app->h = 120;
    app->actors_yoffset = 240;
    app->actors = actors_new();
    land_clear(1, 0, 1, 1);
    land_flip();
    land_display_title("raytracing items");
    items_init();
    actor_types_init();
    land_clear(1, 1, 1, 1);
    land_flip();
    land_display_title("Quest of the Prime Dragons (TINS 2012)");
    parallax_init();
    restart();
}
void runner_done(LandRunner * self) {
    ;
}
static float scrollpos(float w) {
    return fmod(fmod(app->parallax_scroll, w) + w, w);
}
void runner_update(LandRunner * self) {
    if (land_was_resized()) {
        int w = land_display_width();
        int h = land_display_height();
        float sw = w / 640.0;
        float sh = h / 360.0;
        scale = _scramble_min(sw, sh);
    }
    if (land_closebutton()) {
        land_quit();
    }
    if (! land_keybuffer_empty()) {
        int k, u;
        land_keybuffer_next( & k, & u);
        if (k == LandKeyEscape) {
            land_quit();
        }
        if (k == LandKeyFunction + 1) {
            restart();
        }
        if (k == LandKeyFunction + 2) {
            save();
        }
        if (k == LandKeyFunction + 3) {
            load();
        }
        if (k == ' ') {
            special(app->actors->selected);
        }
        if (k == 'p') {
            app->paused ^= 1;
        }
    }
    if (land_mouse_button(0) || land_mouse_button(1)) {
        app->parallax_scroll -= land_mouse_delta_x() / scale;
        app->scroll = scrollpos(app->w);
    }
    app->mx = app->scroll + land_mouse_x() / scale;
    if (app->mx > app->w) {
        app->mx -= app->w;
    }
    app->my = land_mouse_y() / scale - app->actors_yoffset;
    if (! app->paused) {
        actors_tick(app->actors);
        wave_tick();
    }
}
void runner_redraw(LandRunner * self) {
    land_clear(0, 0, 0, 1);
    land_clip(0, 0, 640 * scale, 360 * scale);
    land_reset_transform();
    land_scale(scale, scale);
    parallax_draw();
    for (int i = 0; i < 2; i += 1) {
        land_push_transform();
        land_translate( - app->scroll + 1280 * i, app->actors_yoffset);
        actors_draw(app->actors);
        land_pop_transform();
    }
    if (app->actors->selected) {
        Actor * a = app->actors->selected;
        land_text_pos(0, 0);
        land_color(0, 0, 0, 1);
        land_print("Class: %s", actor_names[a->kind->id]);
        land_print("HP: %.0f / %.0f", a->hp, a->max_hp);
        land_print("Level: %d", a->kind->level);
        land_text_pos(480, 0);
        if (a->special == 900) {
            land_color(1, 1, 0, 1);
        }
        else {
            land_color(0, 0, 1, 1);
        }
        land_print("Space: %s special", actor_names[a->kind->id]);
        land_color(0.5, 0, 0.5, 1);
        land_filled_rectangle(480 - 1, 10, 480 + 120 + 1, 16);
        land_color(1, 0, 1, 1);
        land_filled_rectangle(480, 11, 480 + 120 * a->special / 900, 15);
        if (a->target) {
            Actor * t = a->target;
            land_text_pos(120, 0);
            land_color(1, 0, 0, 1);
            land_print("Class: %s", actor_names[t->kind->id]);
            land_print("HP: %.0f / %.0f", t->hp, t->max_hp);
            land_print("Level: %d", t->kind->level);
        }
    }
    land_text_pos(240, 0);
    land_color(0, 0, 0, 1);
    land_print("%s", wave_info());
    land_text_pos(360, 0);
    land_color(0, 0, 0, 1);
    land_print("F1: Restart");
    land_print("F2: Save");
    land_print("F3: Load");
    land_print("P: Pause");
}
int my_main(void) {
    int w = 640 * scale;
    int h = 360 * scale;
    main_data_path = land_strdup(".");
    land_init();
    land_set_display_parameters(w, h, LAND_MULTISAMPLE | LAND_ANTIALIAS | LAND_RESIZE);
    LandRunner * game_runner = land_runner_new("TINS 2012", runner_init, NULL, runner_update, runner_redraw, NULL, runner_done);
    land_runner_register(game_runner);
    land_set_initial_runner(game_runner);
    land_mainloop();
    land_free(main_data_path);
    return 0;
}
land_use_main(my_main);
