#include "piece.h"

const int Piece::HEIGHT;
const int Piece::WIDTH;
const Piece::TYPE Piece::TYPE_EMPTY;
const int Piece::NUM_PICTURED_TYPES;
const int Piece::NUM_TYPES;
ALLEGRO_BITMAP * Piece::picture [NUM_PICTURED_TYPES];
ALLEGRO_BITMAP * Piece::picture_large [NUM_PICTURED_TYPES];

void Piece::load (void)
{
    static const string picture_name [NUM_PICTURED_TYPES] =
    {
        "bone",
        "wheel",
        "thread",
        "book",
        "brick",
        "cogwheel",
        "chip"
    };

    for (int cur_type = 0; cur_type < NUM_PICTURED_TYPES; cur_type++)
    {
        string file_name;

        file_name = string ("data/") + picture_name[cur_type] +
                    string ("-25.png");
        picture[cur_type] = al_load_bitmap (file_name.c_str ());
        assert (picture[cur_type] != NULL);

        file_name = string ("data/") + picture_name[cur_type] +
                    string ("-100.png");
        picture_large[cur_type] = al_load_bitmap (file_name.c_str ());
        assert (picture_large[cur_type] != NULL);
    }
}

ALLEGRO_BITMAP * Piece::get_picture_large (int picture_type)
{
    return picture_large[picture_type];
}

Piece::Piece (TYPE new_type):
    type (new_type),
    shift_x (0.0),
    shift_y (HEIGHT),
    speed_x (0.0),
    speed_y (0.0),
    state (STATE_FALLING)
{
    do_nothing ();
}

Piece::TYPE Piece::get_type (void)
{
    return type;
}

void Piece::set_type (Piece::TYPE new_type)
{
    type = new_type;
}

void Piece::draw (float x, float y)
{
    static const ALLEGRO_COLOR COLOR [NUM_TYPES + 1] =
    {
        al_map_rgb (  0,   0, 255),
        al_map_rgb (  0, 255,   0),
        al_map_rgb (255,   0,   0),
        al_map_rgb (  0, 255, 255),
        al_map_rgb (255, 255,   0),
        al_map_rgb (255,   0, 255),
        al_map_rgb (128,   0, 255),
        al_map_rgb (  0, 255, 128),
        al_map_rgb (255, 128,   0),
        al_map_rgb (  0, 128, 255),
        al_map_rgb (128, 255,   0),
        al_map_rgb (255,   0, 128),
        al_map_rgb (128, 128, 128)
    };

    if (type == TYPE_EMPTY)
    {
        return;
    }

    int type_num = max (0, min (type, NUM_TYPES));
    if (type_num < NUM_PICTURED_TYPES)
    {
        al_draw_bitmap (picture[type_num], x + 1.0, y + 1.0 - shift_y, 0);
    }
    else
    {
        al_draw_filled_circle (x + WIDTH  * 0.5 + 1.0,
                               y + HEIGHT * 0.5 + 1.0 - shift_y,
                               10.0,
                               COLOR[type_num]);
    }
}

bool Piece::needs_fall (void)
{
    return (state == STATE_FALLING) &&
           (shift_y < 0.0);
}

bool Piece::is_empty (void)
{
    return (type == TYPE_EMPTY);
}

bool Piece::is_fixed (void)
{
    return (state == STATE_FIXED);
}

void Piece::fix (void)
{
    assert ((state == STATE_FALLING) ||
            (state == STATE_FIXED));
    state = STATE_FIXED;
    shift_y = 0.0;
    speed_y = 0.0;
}

void Piece::unfix (void)
{
    assert ((state == STATE_FALLING) ||
            (state == STATE_FIXED));
    state = STATE_FALLING;
}

void Piece::clear (void)
{
    type = TYPE_EMPTY;
    state = STATE_NONE;
}

void Piece::fall_to (Piece & target)
{
    target = *this;
    target.shift_y += HEIGHT;

    type = TYPE_EMPTY;
}

void Piece::move (void)
{
    static const real ACCELERATION = 0.5;
    static const real MAX_SPEED_Y = 5.0;

    if (is_fixed ())
    {
        return;
    }

    shift_y -= speed_y;
    speed_y = min (speed_y + ACCELERATION, MAX_SPEED_Y);
}

void Piece::downgrade (void)
{
    if (type == TYPE_EMPTY)
        return;

//    printf ("type = %d\n", type);
    type--;
//    printf ("-> type = %d\n", type);
    if (type == TYPE_EMPTY)
    {
//        printf ("!\n");
        clear ();
    }
}
