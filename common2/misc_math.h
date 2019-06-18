#pragma once
#include <math.h>
// достало каждый раз эту мелочевку копировать

static double inline sqr(double x)
{ return x*x; }

static inline int round(double x)
{
	return int(floor(x+0.5));
}
