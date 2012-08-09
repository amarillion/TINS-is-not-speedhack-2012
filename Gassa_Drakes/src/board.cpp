#include "board.h"

#include <cmath>
#include <ctime>

#include <cstdio>

const int Board::HEIGHT;
const int Board::WIDTH;
const int Board::SPEED;
const int Board::MAX_SIZE;
const int Board::MIN_COMPONENT_SIZE;
const int Board::MAX_LEVEL;
const int Board::SCORE_LIMIT [Board::MAX_LEVEL] = {    0,
                                                   10000,
                                                   20000,
                                                   30000,
                                                   40000};
ALLEGRO_BITMAP * Board::picture_fill;
int Board::num_prime [];

void Board::load (void)
{
    ALLEGRO_BITMAP * parent = al_get_target_bitmap ();
    picture_fill = al_create_bitmap (Piece::WIDTH - 1, Piece::HEIGHT - 1);
    al_set_target_bitmap (picture_fill);
    al_clear_to_color (fixed_color);
    al_set_target_bitmap (parent);

    num_prime[0] = 0;
    num_prime[1] = 0;
    int cur_prime_num = 0;
    for (int size = 2; size < MAX_SIZE + 1; size++)
    {
        bool is_prime = true;
        for (int k = 2; k * k <= size; k++)
        {
            if (size % k == 0)
            {
                is_prime = false;
                break;
            }
        }

        if (is_prime)
        {
            cur_prime_num++;
            num_prime[size] = cur_prime_num;
        }
    }
}

Board::Board (int new_level, int new_draw_x, int new_draw_y):
    timer (al_create_timer (1.0 / SPEED)),
    random (time (NULL)),
    level (min (new_level, MAX_LEVEL)),
    state (STATE_NONE),
    score (SCORE_LIMIT[min (new_level, MAX_LEVEL) - 1]),
    cur_prime (0),
    draw_x (new_draw_x),
    draw_y (new_draw_y),
    mouse_clicked (false),
    mouse_x (0.0),
    mouse_y (0.0),
    power (0)
{
    assert (timer != NULL);
    init ();
}

void Board::init (void)
{
    for (int row = 0; row < HEIGHT; row++)
    {
        for (int col = 0; col < WIDTH; col++)
        {
            body[row][col] = Piece (Piece::TYPE_EMPTY);
        }
    }
}

void Board::start (void)
{
    assert ((state == STATE_NONE) ||
            (state == STATE_PAUSED));
    state = STATE_RUNNING;

    al_start_timer (timer);
}

void Board::stop (void)
{
    assert ((state == STATE_RUNNING) ||
            (state == STATE_FINISHED));
    state = STATE_PAUSED;

    al_stop_timer (timer);
}

ALLEGRO_TIMER * const Board::get_timer (void)
{
    return timer;
}

void Board::draw (void)
{
    ALLEGRO_BITMAP * parent = al_get_target_bitmap ();
    ALLEGRO_BITMAP * cur = al_create_sub_bitmap (parent, draw_x, draw_y,
                                                 WIDTH  * Piece::WIDTH  + 1,
                                                 HEIGHT * Piece::HEIGHT + 1);
    al_set_target_bitmap (cur);
//    al_clear_to_color (background_color);

    for (int row = 0; row <= HEIGHT; row++)
    {
        al_draw_line (0      * Piece::WIDTH  + 0.5,
                      row    * Piece::HEIGHT + 0.5,
                      WIDTH  * Piece::WIDTH  + 1.5,
                      row    * Piece::HEIGHT + 0.5,
                      border_color,
                      1.0);
    }

    for (int col = 0; col <= WIDTH; col++)
    {
        al_draw_line (col    * Piece::WIDTH  + 0.5,
                      0      * Piece::HEIGHT + 0.5,
                      col    * Piece::WIDTH  + 0.5,
                      HEIGHT * Piece::HEIGHT + 1.5,
                      border_color,
                      1.0);
    }

    for (int row = 0; row < HEIGHT; row++)
    {
        for (int col = 0; col < WIDTH; col++)
        {
            if (body[row][col].is_fixed ())
            {
/*
                al_draw_bitmap (picture_fill,
                                col * Piece::WIDTH  + 1.0,
                                row * Piece::HEIGHT + 1.0,
                                0);
*/
                al_draw_filled_rectangle ((col + 0) * Piece::WIDTH  + 1.0,
                                          (row + 0) * Piece::HEIGHT + 1.0,
                                          (col + 1) * Piece::WIDTH  - 0.0,
                                          (row + 1) * Piece::HEIGHT - 0.0,
                                          fixed_color);
            }
        }
    }

    for (int row = 0; row < HEIGHT; row++)
    {
        for (int col = 0; col < WIDTH; col++)
        {
            body[row][col].draw (col * Piece::WIDTH, row * Piece::HEIGHT);
        }
    }

    for (vector <Drake>::iterator it = drake.begin ();
         it != drake.end (); it++)
    {
        it -> draw ();
    }

    al_set_target_bitmap (parent);
}

void Board::add_random_piece (void)
{
    const int variety = level + 2;
//    const int PERIOD = 200.0 / variety;
    const int MIN_HEIGHT = 5;
    const real PROBABILITY_MULT = 1.0 / 3.0;

    real probability [WIDTH] = {0.0};
    real sum = 0.0;

    for (int col = 0; col < WIDTH; col++)
    {
        int num = 0;
        for (int row = 0; row < HEIGHT; row++)
            if (!body[row][col].is_empty ())
            {
                num++;
            }

        int row = 0;
        if (!body[row][col].is_empty ())
        {
            probability[col] = 0.0;
        }
        else if (num < MIN_HEIGHT)
        {
            probability[col] = 1.0;
        }
        else
        {
            probability[col] = (1.0 - (real) num / HEIGHT) * PROBABILITY_MULT;
        }
        sum += probability[col];
    }

    real roll = random.random_value ((real) WIDTH);
    if (roll >= sum)
        return;

    int row = 0;
    for (int col = 0; col < WIDTH; col++)
    {
        roll -= probability[col];
        if (roll < 0.0)
        {
            if (!body[row][col].is_empty ())
            {
                return;
            }

            int new_type = random.random_value (variety);
            body[row][col] = Piece (new_type);

            return;
        }
    }
}

bool Board::can_fall (int row, int col)
{
    return (row + 1 < HEIGHT) &&
           (body[row + 1][col].is_empty ());
}

void Board::move_piece (int row, int col)
{
    if (body[row][col].is_empty ())
    {
        return;
    }

    if (can_fall (row, col))
    {
        body[row][col].unfix ();
    }

    if (body[row][col].is_fixed ())
    {
        return;
    }

    body[row][col].move ();

    while ((body[row][col].needs_fall ()) && can_fall (row, col))
    {
        body[row][col].fall_to (body[row + 1][col]);
        row++;
    }

    if (body[row][col].needs_fall ())
    {
        body[row][col].fix ();
    }
}

void Board::move (void)
{
    assert (state == STATE_RUNNING);

    add_random_piece ();

    for (int row = HEIGHT - 1; row >= 0; row--)
    {
        for (int col = 0; col < WIDTH; col++)
        {
            move_piece (row, col);
        }
    }

    vector <Drake> next_drake;
    for (vector <Drake>::iterator it = drake.begin ();
         it != drake.end (); it++)
    {
        pair <real, real> p = it -> move ();
        if (p != PAIR_NA)
        {
            int px = p.first;
            int py = p.second;
//            printf ("%d %d\n", px, py);
            if (0 <= px && px < WIDTH  * Piece::WIDTH &&
                0 <= py && py < HEIGHT * Piece::HEIGHT)
            {
                px /= Piece::WIDTH;
                py /= Piece::HEIGHT;
//                printf ("%d %d\n", px, py);
                body[px][py].downgrade ();
            }
            next_drake.push_back (*it);
        }
    }
    drake = next_drake;

    if (is_filled ())
    {
        state = STATE_FINISHED;
    }
}

bool Board::is_filled (void)
{
    int row = 0;
    for (int col = 0; col < WIDTH; col++)
    {
        if (!body[row][col].is_empty () &&
            body[row][col].is_fixed ())
        {
            return true;
        }
    }

    return false;
}

Board::STATE Board::get_state (void)
{
    return state;
}

int Board::get_score (void)
{
    return score;
}

int Board::get_cur_prime (void)
{
    return cur_prime;
}

vector <pair <int, int> > Board::find_component (int start_row, int start_col)
{
    static const int MAX_D = 4;
    static const int D_ROW [MAX_D] = {-1,  0, +1,  0};
    static const int D_COL [MAX_D] = { 0, -1,  0, +1};

    if (body[start_row][start_col].is_empty () ||
        !body[start_row][start_col].is_fixed ())
        return vector <pair <int, int> > ();

    bool visited [HEIGHT] [WIDTH] = {{false}};
    queue <pair <int, int> > position_queue;

    visited[start_row][start_col] = true;
    position_queue.push (make_pair (start_row, start_col));
    vector <pair <int, int> > component;

    while (!position_queue.empty ())
    {
        pair <int, int> cur_pos = position_queue.front ();
        position_queue.pop ();

        int cur_row = cur_pos.first;
        int cur_col = cur_pos.second;
        component.push_back (cur_pos);

        for (int dir = 0; dir < MAX_D; dir++)
        {
            int new_row = cur_row + D_ROW[dir];
            int new_col = cur_col + D_COL[dir];

            if ((new_row < 0) || (HEIGHT <= new_row) ||
                (new_col < 0) || (WIDTH  <= new_col))
            {
                continue;
            }

            if (visited[new_row][new_col] ||
                (body[new_row][new_col].get_type () !=
                 body[cur_row][cur_col].get_type ()))
            {
                continue;
            }

            visited[new_row][new_col] = true;
            position_queue.push (make_pair (new_row, new_col));
        }
    }

    return component;
}

void Board::increase_level (void)
{
    while ((level < MAX_LEVEL) &&
           (score >= SCORE_LIMIT[level]))
    {
        level++;
        max_prime[level - 1] = max (max_prime[level - 1], get_cur_prime ());
    }
}

int Board::get_level (void)
{
    return level;
}

void Board::register_mouse_click (float cur_x, float cur_y)
{
    assert (!mouse_clicked);

    if ((cur_y < draw_y) || (draw_x + HEIGHT * Piece::HEIGHT < cur_y) ||
        (cur_x < draw_x) || (draw_x + WIDTH  * Piece::WIDTH  < cur_x))
    {
        return;
    }

    int row = (cur_y - draw_y) / Piece::HEIGHT;
    int col = (cur_x - draw_x) / Piece::WIDTH;

    vector <pair <int, int> > component = find_component (row, col);
    int size = component.size ();
    if (size < MIN_COMPONENT_SIZE)
    {
        return;
    }

    int add_score = level * size * (size - 1);
    if (num_prime[size] <= 0)
    {
        add_score /= 2;
    }
    score += add_score;
    increase_level ();

    for (vector <pair <int, int> >::iterator it = component.begin ();
         it != component.end ();
         it++)
    {
        body[it -> first][it -> second].clear ();
    }

    if (num_prime[size] <= 0)
    {
        return;
    }

    mouse_clicked = true;
    mouse_x = cur_x;
    mouse_y = cur_y;
    power = num_prime[size];
}

void Board::register_mouse_release (float cur_x, float cur_y)
{
    if (!mouse_clicked)
    {
        return;
    }
    mouse_clicked = false;

    float mouse_x_to = cur_x;
    float mouse_y_to = cur_y;

    real dir;
    if (mouse_x_to == mouse_x && mouse_y_to == mouse_y)
    {
        dir = random.random_value (-M_PI, +M_PI);
    }
    else
    {
        dir = atan2 (mouse_y_to - mouse_y, mouse_x_to - mouse_x);
    }

    drake.push_back (Drake (mouse_x - draw_x, mouse_y - draw_y,
                            dir, power * 3 * level));
}
