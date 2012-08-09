#ifndef __board_h__
#define __board_h__

#include "piece.h"
#include "random.h"
#include "global.h"

#include <math.h>

const pair <real, real> PAIR_NA = make_pair (-256.0, -256.0);

class Drake
{
public:
    static const int WIDTH  = 100;
    static const int HEIGHT = 100;

    real x, y, dir;
    int time;

    Drake (real new_x, real new_y, real new_dir, int new_time):
        x (new_x),
        y (new_y),
        dir (new_dir),
        time (new_time)
    {
        do_nothing ();
    }

    void draw (void)
    {
        al_draw_rotated_bitmap (dragon_flame, WIDTH / 2, HEIGHT / 2,
                                x, y, M_PI * 0.5 + dir, 0);
    }

    pair <real, real> move (void)
    {
        time--;
        if (time <= 0)
        {
            return PAIR_NA;
        }

        real new_dir = dir - M_PI / 4 +
                       M_PI / 2 * global_random.random_value (1.0);
        real dir_distance = global_random.random_value (time * 5.0);
        return make_pair (y + dir_distance * sin (new_dir),
                          x + dir_distance * cos (new_dir));
    }
};

class Board
{
public:
    enum STATE {STATE_NONE, STATE_RUNNING, STATE_PAUSED, STATE_FINISHED};
    static const int HEIGHT = 20;
    static const int WIDTH  = 20;
    static const int SPEED  = 50;
    static const int MAX_SIZE = HEIGHT * WIDTH;
    static const int MIN_COMPONENT_SIZE = 2;
    static const int MAX_LEVEL = 5;
    static const int SCORE_LIMIT [MAX_LEVEL];

private:
    static ALLEGRO_BITMAP * picture_fill;
    static int num_prime [MAX_SIZE + 1];

    Piece body [HEIGHT] [WIDTH];
    ALLEGRO_TIMER * timer;
    Random random;
    int level;
    STATE state;
    int score;
    int cur_prime;
    int draw_x, draw_y;
    bool mouse_clicked;
    float mouse_x, mouse_y;
    int power;
    vector <Drake> drake;

    void add_random_piece (void);
    bool can_fall (int, int);
    void move_piece (int, int);
    vector <pair <int, int> > find_component (int, int);
    void increase_level (void);

public:
    static void load (void);

    Board (int, int, int);
    void init (void);
    void start (void);
    void stop (void);
    ALLEGRO_TIMER * const get_timer (void);
    void draw (void);
    void move (void);
    bool is_filled (void);
    STATE get_state (void);
    int get_score (void);
    int get_level (void);
    int get_cur_prime (void);

    void register_mouse_click (float, float);
    void register_mouse_release (float, float);
};

#endif // __board_h__
