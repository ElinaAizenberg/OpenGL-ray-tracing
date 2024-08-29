#ifndef PROJECT_6_COMMON_H
#define PROJECT_6_COMMON_H

#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <cstdlib>


using std::make_shared;
using std::shared_ptr;

// Constants
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

inline double degrees_to_radians(double degrees)
/** Converts degrees to radians. */
{
    return degrees * pi / 180.0;
}

inline double random_double()
/** Returns a random real in [0,1). */
{
    return std::rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max)
/** Returns a random real in [min,max):
 * 1. generate a random number in [0,1);
 * 2. stretch to the desired range size;
 * 3. shift the result so that the range starts at min and extends to (but does not include) max. */
{
    return min + (max-min)*random_double();
}

// Common Headers
#include "color.h"
#include "interval.h"
#include "ray.h"
#include "vec3.h"

#endif //PROJECT_6_COMMON_H
