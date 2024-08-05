#ifndef PROJECT_6_COLOR_H
#define PROJECT_6_COLOR_H

#include "vec3.h"
#include "interval.h"


using color = vec3;

void write_color(std::ostream& out, const color& pixel_color) {
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    // Translate the [0,1] component values to the byte range [0,255].
    // The multiplication by 255.999 ensures that the maximum value of 1.0 maps to 255 instead of 256.
    static const interval intensity(0.000, 0.999);
    int rbyte = int(256 * intensity.clamp(r));
    int gbyte = int(256 * intensity.clamp(g));
    int bbyte = int(256 * intensity.clamp(b));


    // Write out the pixel color components.
    out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}

#endif //PROJECT_6_COLOR_H
