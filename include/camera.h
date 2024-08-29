
#ifndef PROJECT_6_CAMERA_H
#define PROJECT_6_CAMERA_H

#include "common.h"

#include "hittable.h"
#include "material.h"

class Camera{
public:
    double aspect_ratio      = 1.0;  // Ratio of image width over height
    int    image_width       = 100;  // Rendered image width in pixel count
    int    samples_per_pixel = 10;   // Count of random samples for each pixel
    int    max_depth         = 10;   // Maximum number of ray bounces into scene

    double vfov     = 90;              // Vertical view angle (field of view) - visual angle from edge to edge of the rendered image
    point3 look_from = point3(0,0,0);   // Point camera is looking from
    point3 look_at   = point3(0,0,-1);  // Point camera is looking at
    vec3   view_up      = vec3(0,1,0);     // Camera-relative "up" direction

    double defocus_angle = 0;  // Variation angle of rays through each pixel
    double focus_dist = 10;    // Distance from camera lookfrom point to plane of perfect focus
    /** Focus distance is not usually the same as the focal length.
     * The focal length is the distance between the camera center and the image plane.
     * The focus distance is the distance between the camera center and the plane where everything is in perfect focus.
     * For this model, however, these two will have the same value, as we will put our pixel grid right on the focus plane.
     */

    void render(const hittable& world)
    /** Renders 3D scene with world objects.*/
    {
        initialize();

        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        for (int j = 0; j < image_height; j++)
        {
            std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;

            for (int i = 0; i < image_width; i++)
            {
                color pixel_color(0,0,0);

                // to perform anti-aliasing we need to send multiple rays into the square region centered at the pixel
                // to generate several samples for each pixel
                // More about anti-aliasing here: https://learnopengl.com/Advanced-OpenGL/Anti-Aliasing
                for (int sample = 0; sample < samples_per_pixel; sample++)
                {
                    ray new_ray = generate_ray(i, j);
                    pixel_color += define_ray_color(new_ray, max_depth, world);
                }
                // pixel color to be written in the file is the sum of samples' color values per pixel divided by number of samples per pixel
                write_color(std::cout, pixel_samples_scale * pixel_color);
            }
        }

        std::clog << "\rDone.                 \n";
    }

private:
    int    image_height;         // Rendered image height
    double pixel_samples_scale;  // Color scale factor for a sum of pixel samples
    point3 center;               // Camera center
    point3 pixel00_loc;          // Location of pixel 0, 0
    vec3   pixel_delta_u;        // Offset to pixel to the right
    vec3   pixel_delta_v;        // Offset to pixel below
    vec3   view_direction_opposite, camera_right, camera_up; // Camera frame basis vectors

    vec3   defocus_disk_u;       // Defocus disk horizontal radius
    vec3   defocus_disk_v;       // Defocus disk vertical radius


    void initialize()
    /** Initializes Camera parameters for further rendering. */
    {
        image_height = int(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height; // the resulting height should be at least 1.

        pixel_samples_scale = 1.0 / samples_per_pixel;

        // Camera center is a point in 3D space from which all scene rays will originate.
        // The vector from the camera center to the viewport center will be orthogonal to the viewport.
        center = look_from;

        // Determine viewport dimensions.
        // The viewport is a virtual rectangle in the 3D world that contains the grid of image pixel locations.
        auto theta = degrees_to_radians(vfov); // field of view in radians
        auto h = std::tan(theta/2);
        auto viewport_height = 2 * h * focus_dist;
        // We don't use 'aspect_ratio' because it's an ideal ration and actual differ might differ from 'aspect_ratio'.
        // In order for the viewport proportions to exactly match image proportions, we use the calculated image aspect ratio.
        auto viewport_width = viewport_height * (double(image_width)/image_height);

        // Calculate the complete orthonormal basis vectors for the camera coordinate frame.
        view_direction_opposite = unit_vector(look_from - look_at);
        camera_right = unit_vector(cross(view_up, view_direction_opposite));
        camera_up = cross(view_direction_opposite, camera_right);

        // Calculate the vectors across the horizontal and down the vertical viewport edges.
        // We start at the upper left pixel (pixel 0,0), scan left-to-right across each row, and then scan row-by-row, top-to-bottom.
        // To help navigate the pixel grid, we use a vector from the left edge to the right edge 'viewport_u',
        // and a vector from the upper edge to the lower edge 'viewport_v'.
        vec3 viewport_u = viewport_width * camera_right;    // Vector across viewport horizontal edge
        vec3 viewport_v = viewport_height * -camera_up;  // Vector down viewport vertical edge

        // Calculate the horizontal and vertical delta vectors from pixel to pixel.
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // Calculate the location of the upper left pixel.
        // Moves from the center of the view plane to the actual view plane at focus_dist distance,
        // shifts the position to the left and upper edge of the view plane.
        auto viewport_upper_left = center - (focus_dist * view_direction_opposite) - viewport_u/2 - viewport_v/2;
        // Moves the location from the upper-left corner of the viewport to the center of the pixel.
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

        // Calculate the camera defocus disk basis vectors.
        // defocus radius is calculated the same way as half of the viewport height
        auto defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
        defocus_disk_u = camera_right * defocus_radius;
        defocus_disk_v = camera_up * defocus_radius;
    }

    color define_ray_color(const ray& in_ray, int depth, const hittable& world) const
    /** Calculates a pixel color value by following the lifecycle of the ray until it fails to hit any object or
     * it reaches the maximum number of ray bounces.*/
    {
        // If we've exceeded the ray bounce limit, no more light is gathered.
        // If the maximum number of ray bounces is not set, ray bouncing stops when ray fails to hit anything.
        if (depth <= 0)
            return {0,0,0};

        hit_record record;

        // interval starts with 0.001 to fix shadow acne.
        /** Due to some small numerical errors, introduced by the finite precision of numbers,
         * sometimes the intersection point is not directly above the surface, but slightly below.
         * When this happens and a shadow ray is cast in the light direction, instead of intersecting no object at all
         * or some other object above the object's surface, the shadow ray intersects the surface from which it is cast.
         * In other words, we have a case of self-intersection, which means that a ray will find the nearest surface at t=0.00000001.
         * The simplest hack to address this is just to ignore hits that are very close to the calculated intersection point.
 */
        if (world.hit(in_ray, interval(0.001, infinity), record))
        {
            ray scattered;
            color attenuation;

            if (record.hit_material->scatter(in_ray, record, attenuation, scattered))
                // recursive function stops when depth (maximum number of ray bounces) equals 0.
                return attenuation * define_ray_color(scattered, depth-1, world);

            return {0,0,0};
        }

        // implementation of a simple gradient
        vec3 unit_direction = unit_vector(in_ray.direction()); // normalizes ray vector
        // y() function extracts the vertical component of the direction vector.
        // This value tells us how much the ray is pointing upwards or downwards.
        // Then maps the y component from the range [-1, 1] (which is possible for a unit vector) to the range [0, 1].
        auto a = 0.5*(unit_direction.y() + 1.0);

        // returns the color which is a blend of white and sky color
        // when a = 0.0 (downward direction), the result is purely white.
        // when a = 1.0 (upward direction), the result is purely the sky color.
        return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);
    }

    ray generate_ray(int i, int j) const
    /** Constructs a camera ray originating from the defocus disk and directed at a randomly sampled point
     * around the pixel location i, j. */
    {
        auto offset = sample_square(); // generates a random sample point within the unit square centered at the origin
        auto pixel_sample = pixel00_loc
                            + ((i + offset.x()) * pixel_delta_u)
                            + ((j + offset.y()) * pixel_delta_v);

        // Without defocus blur, all scene rays originate from the camera center (or lookfrom).
        // The larger the radius of a defocus disk, the greater the defocus blur.
        auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
        auto ray_direction = pixel_sample - ray_origin;

        return {ray_origin, ray_direction};
    }

    point3 defocus_disk_sample() const
    /** Returns a random point in the camera defocus disk. */
    {
        auto p = random_in_unit_disk();
        return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }

    static vec3 sample_square()
    /** Returns the vector to a random point in the [-0.5,-0.5]-[+0.5,+0.5] unit square.*/
    {
        return {random_double() - 0.5, random_double() - 0.5, 0};
    }


};
#endif //PROJECT_6_CAMERA_H
