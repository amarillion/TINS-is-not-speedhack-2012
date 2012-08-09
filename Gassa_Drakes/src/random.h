#ifndef __random_h__
#define __random_h__

#include <stdint.h>

#include "global.h"

class Random
{
private:
    static const uint32_t DEFAULT_SEED = 12345;
    static const uint32_t LCG_MULT = 1664525;
    static const uint32_t LCG_ADD = 1013904223;
    static const real SCALE_TO_REAL = 1.0 / 65536.0 / 65536.0;

    uint32_t state;

public:
    Random (uint32_t = DEFAULT_SEED);
    void next (void);
    int random_value (int);
    uint32_t random_value (uint32_t);
    real random_value (real);
    real random_value (real, real);
};

extern Random global_random;

#endif // __random_h__
