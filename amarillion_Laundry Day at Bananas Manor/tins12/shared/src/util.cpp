#include "util.h"

int bound (int _min, int val, int _max)
{
	return min(max (_min, val), _max);
}

int min (int a, int b)
{
	return a < b ? a : b;
}

int max (int a, int b)
{
	return a > b ? a : b;
}
