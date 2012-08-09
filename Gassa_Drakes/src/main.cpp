#include <cmath>
#include <cstdio>
#include <ctime>
#include <stdint.h>

#include "board.h"
#include "piece.h"
#include "global.h"

const int MAX_X = 800, MAX_Y = 600;
const int BOARD_X = 150, BOARD_Y = 50;
const int FPS = 60;

ALLEGRO_COLOR border_color;
ALLEGRO_COLOR fixed_color;
ALLEGRO_COLOR background_color;

ALLEGRO_DISPLAY * display;
ALLEGRO_EVENT_QUEUE * event_queue;
ALLEGRO_TIMER * global_timer;

ALLEGRO_FONT * global_font_18;
ALLEGRO_FONT * global_font_24;
ALLEGRO_FONT * global_font_72;

ALLEGRO_BITMAP * dragon_flame;
ALLEGRO_BITMAP * dragon;
ALLEGRO_BITMAP * flame;

Random global_random (time (NULL));

const string save_file_name = "data/save.txt";

const int BUTTON_FIRST    = 150;
const int BUTTON_WIDTH    = 200;
const int BUTTON_HEIGHT   = 48;
const int BUTTON_DISTANCE = 64;

int max_prime [Board::MAX_LEVEL] = {0, -1, -1, -1, -1};
int max_score = 0;

void save (void)
{
    FILE * save_file = fopen (save_file_name.c_str (), "wt");

    if (save_file == NULL)
    {
        assert (false);
        return;
    }

    for (int level = 0; level < Board::MAX_LEVEL; level++)
    {
        fprintf (save_file, "%d\n", max_prime[level]);
    }

    fprintf (save_file, "%d\n", max_score);

    fclose (save_file);
}

void load (void)
{
    FILE * save_file = fopen (save_file_name.c_str (), "rt");

    if (save_file == NULL)
    {
        save ();
        return;
    }

    for (int level = 0; level < Board::MAX_LEVEL; level++)
    {
        fscanf (save_file, " %d", &max_prime[level]);
    }

    fscanf (save_file, " %d", &max_score);

    fclose (save_file);
}

void happy_end (void)
{
    save ();

    al_destroy_font (global_font_18);
    al_destroy_font (global_font_24);
    al_destroy_font (global_font_72);

    al_stop_timer (global_timer);
    al_destroy_timer (global_timer);

    al_destroy_display (display);

    al_destroy_event_queue (event_queue);

    al_shutdown_primitives_addon ();

    al_shutdown_image_addon ();

    al_shutdown_font_addon ();

    al_shutdown_ttf_addon ();
    exit (EXIT_SUCCESS);
}

void init (void)
{
    bool res;

    res = al_init ();
    assert (res);

    res = al_init_primitives_addon ();
    assert (res);

    res = al_init_image_addon ();
    assert (res);

    al_init_font_addon ();

    res = al_init_ttf_addon ();
    assert (res);

    res = al_install_keyboard ();
    assert (res);

    res = al_install_mouse ();
    assert (res);

    global_timer = al_create_timer (1.0 / FPS);
    assert (global_timer != NULL);
    al_start_timer (global_timer);

    display = al_create_display (MAX_X, MAX_Y);
    assert (display != NULL);

    event_queue = al_create_event_queue ();
    assert (event_queue != NULL);

    al_register_event_source (event_queue,
                              al_get_keyboard_event_source ());
    al_register_event_source (event_queue,
                              al_get_mouse_event_source ());
    al_register_event_source (event_queue,
                              al_get_display_event_source (display));
    al_register_event_source (event_queue,
                              al_get_timer_event_source (global_timer));

    Piece::load ();

    Board::load ();

    global_font_18 = al_load_ttf_font ("data/Inconsolata.otf", 18, 0);
    assert (global_font_18 != NULL);

    global_font_24 = al_load_ttf_font ("data/Inconsolata.otf", 24, 0);
    assert (global_font_24 != NULL);

    global_font_72 = al_load_ttf_font ("data/Inconsolata.otf", 72, 0);
    assert (global_font_72 != NULL);

    border_color     = al_map_rgba (  0,  63, 127,  64);
    fixed_color      = al_map_rgba (  0,  47,  95,  64);
    background_color = al_map_rgba (  0,  31,  63, 192);

    load ();

    dragon_flame = al_load_bitmap ("data/dragonflame-100.png");
    assert (dragon_flame != NULL);
}

void draw_background (int level_num)
{
    al_clear_to_color (al_map_rgb (0, 0, 0));
/*
    al_clear_to_color (al_map_rgb (8  * 3,
                                   4  * 3,
                                   12 * 3));
*/
    static const int BACK_WIDTH  = 100;
    static const int BACK_HEIGHT = 100;
    static const float BACK_SPEED [Piece::NUM_PICTURED_TYPES] =
        {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0};

    typedef vector <pair <real, real> > frame_container;
    static frame_container back_frame [Piece::NUM_PICTURED_TYPES];
    static int back_pause [Piece::NUM_PICTURED_TYPES] =
        {100, 200, 300, 400, 500, 600, 700};

    const int variety = level_num + 2;

    for (int layer = variety - 1; layer >= 0; layer--)
    {
        back_pause[layer]--;

        if (back_pause [layer] <= 0)
        {
            back_frame[layer].push_back (make_pair (BOARD_X +
                global_random.random_value (Board::WIDTH * Piece::WIDTH -
                                            BACK_WIDTH + 1), -BACK_HEIGHT));
            back_pause[layer] += 100 + global_random.random_value
                ((variety + 1) * 100);
        }

        for (frame_container::iterator it = back_frame[layer].begin ();
             it != back_frame[layer].end (); it++)
        {
            al_draw_bitmap (Piece::get_picture_large(layer),
                            it -> first, it -> second, 0);
            it -> second += BACK_SPEED[layer];
        }

        while (!back_frame[layer].empty () &&
               (back_frame[layer].begin () -> second >= MAX_Y))
        {
            back_frame[layer].erase (back_frame[layer].begin ());
        }
    }
}

void draw_level (int level_num, Board * board)
{
    al_draw_rectangle (BOARD_X - 3,
                       BOARD_Y - 3,
                       BOARD_X + Board::WIDTH  * Piece::WIDTH  + 4,
                       BOARD_Y + Board::HEIGHT * Piece::HEIGHT + 4,
                       border_color,
                       6);

    al_draw_filled_rectangle (BOARD_X,
                              BOARD_Y,
                              BOARD_X + Board::WIDTH  * Piece::WIDTH,
                              BOARD_Y + Board::HEIGHT * Piece::HEIGHT,
                              background_color);

    al_draw_textf (global_font_24, al_map_rgb (255, 255, 255),
                   MAX_X / 2 - 100, 10,
                   ALLEGRO_ALIGN_CENTRE, "Level %d", level_num);

    al_draw_textf (global_font_24, al_map_rgb ( 63, 191, 255),
                   MAX_X / 2 + 100, 10,
                   ALLEGRO_ALIGN_CENTRE, "Score: %07d", board -> get_score ());
}

void pause (void)
{
    al_flush_event_queue (event_queue);

    bool to_end = false;
    while (!to_end)
    {
        ALLEGRO_EVENT current_event;
        al_wait_for_event (event_queue, &current_event);

        switch (current_event.type)
        {
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                happy_end ();
                break;

            case ALLEGRO_EVENT_KEY_DOWN:
                to_end = true;
                break;

            case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
                to_end = true;
                break;

            default:
                do_nothing ();
                break;
        }
    }

    al_flush_event_queue (event_queue);
}

void play_level (int level_num)
{
    al_flush_event_queue (event_queue);

    Board board (level_num, BOARD_X, BOARD_Y);
    board.init ();
    level_num = board.get_level ();
    al_register_event_source (event_queue,
                              al_get_timer_event_source (board.get_timer ()));

    al_clear_to_color (al_map_rgb (0, 0, 0));
/*
    al_clear_to_color (al_map_rgb (8  * level_num,
                                   4  * level_num,
                                   12 * level_num));
*/
    board.start ();
    bool to_end = false;
    while (!to_end)
    {
        ALLEGRO_EVENT current_event;
        al_wait_for_event (event_queue, &current_event);

        switch (current_event.type)
        {
            case ALLEGRO_EVENT_TIMER:
                if (current_event.timer.source == global_timer)
                {
                    draw_background (level_num);
                    draw_level (level_num, &board);
                    board.draw ();
                    al_flip_display ();
                }
                else if (current_event.timer.source == board.get_timer ())
                {
                    board.move ();
                    level_num = board.get_level ();
                    if (board.is_filled ())
                    {
                        to_end = true;
                    }
                }
                else
                {
                    assert (false);
                }
                break;

            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                happy_end ();
                break;

            case ALLEGRO_EVENT_KEY_DOWN:
                switch (current_event.keyboard.keycode)
                {
                    case ALLEGRO_KEY_ESCAPE:
                        to_end = true;
                        break;
                }
                break;

            case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
                board.register_mouse_click (current_event.mouse.x,
                                            current_event.mouse.y);
                break;

            case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
                board.register_mouse_release (current_event.mouse.x,
                                              current_event.mouse.y);
                break;

            default:
                do_nothing ();
                break;
        }
    }
    board.stop ();

    max_score = max (max_score, board.get_score ());
    save ();

    al_draw_text (global_font_72, al_map_rgb (255, 127,  63),
                   MAX_X / 2, (MAX_Y - 72) / 2 - 50,
                   ALLEGRO_ALIGN_CENTRE, "GAME OVER");

    al_draw_text (global_font_24, al_map_rgb (255, 255, 255),
                   MAX_X / 2, (MAX_Y - 24) / 2 + 50,
                   ALLEGRO_ALIGN_CENTRE, "Press any key...");
    al_flip_display ();

    pause ();

    al_unregister_event_source (event_queue,
                                al_get_timer_event_source (board.get_timer ()));

    al_flush_event_queue (event_queue);
}

void draw_menu (void)
{
    al_clear_to_color (al_map_rgb (0, 0, 0));

    al_draw_text (global_font_72, al_map_rgb (255, 191, 127),
                  MAX_X / 2,
                  50,
                  ALLEGRO_ALIGN_CENTRE, "DRAKES");

    for (int level = 0; level <= Board::MAX_LEVEL; level++)
    {
        if (max_prime[level] < 0)
        {
            continue;
        }

        al_draw_rectangle ((MAX_X - BUTTON_WIDTH) / 2,
                           BUTTON_FIRST + level * BUTTON_DISTANCE,
                           (MAX_X + BUTTON_WIDTH) / 2,
                           BUTTON_FIRST + level * BUTTON_DISTANCE +
                                          BUTTON_HEIGHT,
                           border_color,
                           6);

        al_draw_filled_rectangle ((MAX_X - BUTTON_WIDTH) / 2,
                                  BUTTON_FIRST + level * BUTTON_DISTANCE,
                                  (MAX_X + BUTTON_WIDTH) / 2,
                                  BUTTON_FIRST + level * BUTTON_DISTANCE +
                                                 BUTTON_HEIGHT,
                                  fixed_color);

        if (level < Board::MAX_LEVEL)
        {
            al_draw_textf (global_font_24, al_map_rgb (127, 191, 255),
                           MAX_X / 2,
                           BUTTON_FIRST + level * BUTTON_DISTANCE +
                                          (BUTTON_HEIGHT - 24) / 2,
                           ALLEGRO_ALIGN_CENTRE, "Level %d", level + 1);
        }
        else
        {
            al_draw_textf (global_font_24, al_map_rgb (127, 191, 255),
                           MAX_X / 2,
                           BUTTON_FIRST + level * BUTTON_DISTANCE +
                                          (BUTTON_HEIGHT - 24) / 2,
                           ALLEGRO_ALIGN_CENTRE, "EXIT");
        }

/*
        al_draw_textf (global_font_24, al_map_rgb ( 63, 191, 255),
                       MAX_X / 2 + 100, 10,
                       ALLEGRO_ALIGN_CENTRE, "Score: %05d",
                       board -> get_score ());
*/
    }

    al_draw_textf (global_font_24, al_map_rgb (255,   0,   0),
                   MAX_X / 2,
                   BUTTON_FIRST + (Board::MAX_LEVEL + 1) * BUTTON_DISTANCE +
                                  BUTTON_HEIGHT / 2 - 24,
                   ALLEGRO_ALIGN_CENTRE, "High Score: %07d",
                   max_score);
}

void main_menu (void)
{
    al_flush_event_queue (event_queue);

    int level_num;

    draw_menu ();
    al_flip_display ();

    bool to_end = false;
    while (!to_end)
    {
        ALLEGRO_EVENT current_event;
        al_wait_for_event (event_queue, &current_event);

        switch (current_event.type)
        {
            case ALLEGRO_EVENT_TIMER:
                draw_menu ();
                al_flip_display ();
                break;

            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                happy_end ();
                break;

            case ALLEGRO_EVENT_KEY_DOWN:
                switch (current_event.keyboard.keycode)
                {
                    case ALLEGRO_KEY_ESCAPE:
                        to_end = true;
                        break;
                }
                break;

            case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
                level_num = -1;

                if ((MAX_X - BUTTON_WIDTH) / 2 <= current_event.mouse.x &&
                    current_event.mouse.x <= (MAX_X + BUTTON_WIDTH) / 2)
                {
                    int pos_y = current_event.mouse.y - BUTTON_FIRST;
                    if (pos_y >= 0)
                    {
                        if ((0 <= pos_y % BUTTON_DISTANCE) &&
                            (pos_y % BUTTON_DISTANCE <= BUTTON_HEIGHT))
                        {
                            level_num = pos_y / BUTTON_DISTANCE;
                        }
                    }
                }

                if (0 <= level_num && level_num < Board::MAX_LEVEL)
                {
                    if (max_prime[level_num] >= 0)
                    {
                        play_level (level_num + 1);
                        draw_menu ();
                        al_flip_display ();
                    }
                }
                else if (level_num == Board::MAX_LEVEL)
                {
                    happy_end ();
                }
                break;

            default:
                do_nothing ();
                break;
        }
    }

    al_flush_event_queue (event_queue);
}

int main (void)
{
    init ();
    main_menu ();
    happy_end ();
    return 0;
}
