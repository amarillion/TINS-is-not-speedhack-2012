#include "random.h"

Random::Random (uint32_t seed): state (seed)
{
}

void Random::next (void)
{
    state = state * LCG_MULT + LCG_ADD;
}

int Random::random_value (int range)
{
    next ();
    return ((int64_t) state * range) >> 32LL;
}

uint32_t Random::random_value (uint32_t range)
{
    next ();
    return ((uint64_t) state * range) >> 32ULL;
}

real Random::random_value (real range)
{
    next ();
    return state * range * SCALE_TO_REAL;
}

real Random::random_value (real left, real right)
{
    return left + random_value (right - left);
}
