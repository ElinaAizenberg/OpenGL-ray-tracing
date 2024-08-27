#ifndef PROJECT_6_COLOR_H
#define PROJECT_6_COLOR_H

#include "vec3.h"
#include "interval.h"

// color is an alias for vec3 (alternative name to the vec3 type)
using color = vec3;

/** In graphics programming, color values are often in linear space, meaning the intensity of light scales linearly with the color value.
 * Human vision perceives brightness in a nonlinear way, so gamma correction is applied to make the image look more natural on a display.
 * Almost all computer programs assume that an image is “gamma corrected” before being written into an image file.
 * Gamma correction is typically the process of raising the color components to a power (often around 2.2 for encoding).
*/

inline double linear_to_gamma_corrected_value(double linear_component)
/** The idea of gamma correction is to apply the inverse of the monitor's gamma to the final output color before displaying to the monitor.
 * We multiply each of the linear output colors by this inverse gamma curve (making them brighter)
 * and as soon as the colors are displayed on the monitor, the monitor's gamma curve is applied and the resulting colors become linear.
 * This transformation is done by taking the square root (std::sqrt()), which is a common approximation for gamma correction with an exponent of 2.0.
 * More about gamma correction: https://learnopengl.com/Advanced-Lighting/Gamma-Correction
*/
{
    if (linear_component > 0)
        return std::sqrt(linear_component);

    return 0;
}

void write_color(std::ostream& out, const color& pixel_color)
/** Writes a single pixel's color out to the standard output stream. */
{
    auto red = pixel_color.x();
    auto green = pixel_color.y();
    auto blue = pixel_color.z();

    // Apply a linear to gamma transform for gamma 2
    red = linear_to_gamma_corrected_value(red);
    green = linear_to_gamma_corrected_value(green);
    blue = linear_to_gamma_corrected_value(blue);

    // Static constant member 'intensity' is a constant shared by all instances of the interval class
    static const interval intensity(0.000, 0.999);

    // After clamping, the [0,1) component values is scaled to the range [0, 255].
    // Casting a floating-point number to an integer in C++, the conversion is performed by truncating the decimal part.
    // This means the fractional part is discarded, and only the integer part is kept.
    int red_byte = int(256 * intensity.clamp(red));
    int green_byte = int(256 * intensity.clamp(green));
    int blue_byte = int(256 * intensity.clamp(blue));

    // Write out the pixel color components.
    out << red_byte << ' ' << green_byte << ' ' << blue_byte << '\n';
}

#endif //PROJECT_6_COLOR_H
