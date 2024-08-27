#ifndef PROJECT_6_INTERVAL_H
#define PROJECT_6_INTERVAL_H

#include "common.h"

class interval {
public:
    double min, max;

    interval() : min(+infinity), max(-infinity) {} // Default interval is empty

    interval(double min, double max) : min(min), max(max) {}

    double size() const
    /** Returns the size of the interval. */
    {
        return max - min;
    }

    bool contains(double x) const
    /** Returns if the interval contains the variable including border values. */
    {
        return min <= x && x <= max;
    }

    bool surrounds(double x) const
    /** Returns if the interval contains the variable excluding border values. */
    {
        return min < x && x < max;
    }

    double clamp(double x) const
    /** Constrains a value within a specified range.*/
    {
        if (x < min) return min;
        if (x > max) return max;
        return x;
    }

    // By defining these constants statically, they can be accessed directly from the class
    // without needing to create instances of interval.
    // 'static const' ensures that all parts of the program use the same, consistent instances of these intervals.
    static const interval empty, universe;
};

// C++ requires that static members be defined outside the class to allocate storage for them.
const interval interval::empty    = interval(+infinity, -infinity);
const interval interval::universe = interval(-infinity, +infinity);



#endif //PROJECT_6_INTERVAL_H
