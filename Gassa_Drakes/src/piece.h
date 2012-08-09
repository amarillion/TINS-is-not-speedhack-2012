#ifndef __piece_h__
#define __piece_h__

#include "global.h"

class Piece
{
public:
    enum STATE {STATE_NONE, STATE_FALLING, STATE_FIXED,
                STATE_FLYING, STATE_EXPLODING};
    static const int HEIGHT = 25;
    static const int WIDTH  = 25;

    typedef int TYPE;
    static const TYPE TYPE_EMPTY = -1;

    static const int NUM_PICTURED_TYPES = 7;
    static const int NUM_TYPES = 12;

private:
    static ALLEGRO_BITMAP * picture [NUM_PICTURED_TYPES];
    static ALLEGRO_BITMAP * picture_large [NUM_PICTURED_TYPES];

    TYPE type;
    real shift_x, shift_y;
    real speed_x, speed_y;
    STATE state;

public:
    static void load (void);
    static ALLEGRO_BITMAP * get_picture_large (int);

    Piece (TYPE = TYPE_EMPTY);
    TYPE get_type (void);
    void set_type (TYPE);
    void draw (float, float);
    bool needs_fall (void);
    bool is_empty (void);
    bool is_fixed (void);
    void fix (void);
    void unfix (void);
    void clear (void);
    void fall_to (Piece &);
    void move (void);
    void downgrade (void);
};

#endif // __piece_h__
