
#ifndef PROJECT_6_RAY_H
#define PROJECT_6_RAY_H

#include "vec3.h"

/** Main idea of a ray tracer is that it sends rays through pixels and computes the color seen in the direction of those rays.
 * Main steps:
 * 1. Calculate the ray from the “eye” through the pixel;
 * 2. Determine which objects the ray intersects;
 * 3. Compute a color for the closest intersection point.
 *
 * A ray is a function P(t)=A+tb, where:
 * P - 3D position along a line ;
 * A - the ray origin;
 * b - the ray direction.
*/

class ray{
public:
    ray() = default;
    ray(const point3& origin, const vec3& direction) : ray_origin(origin), ray_direction(direction) {}

    const point3& origin() const  { return ray_origin; }
    const vec3& direction() const { return ray_direction; }

    point3 at(double t) const
    /** Represents ray function P(t)=A+tb. */
    {
        return ray_origin + (t * ray_direction);
    }

private:
    point3 ray_origin;
    vec3 ray_direction;
};


#endif //PROJECT_6_RAY_H
