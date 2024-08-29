#include "include/common.h"
#include "include/camera.h"

#include "include/common.h"


#include "include/camera.h"
#include "include/hittable.h"
#include "include/hittable_list.h"
#include "include/material.h"
#include "include/sphere.h"


int main(){
    hittable_list world;

    auto ground_material = make_shared<lambertian>(color(0.4, 0.6, 0.6));
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));

    for (int a = -6; a < 6; a++) {
        for (int b = -6; b < 6; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.7) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
                else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
                else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 2), 1.0, material1));

    auto material3 = make_shared<metal>(color(0.7, 0.5, 0.8), 0.1);
    world.add(make_shared<sphere>(point3(4, 1.1, 0), 1.1, material3));

    Camera camera;

    camera.aspect_ratio      = 16.0 / 9.0;
    camera.image_width       = 1200;
    camera.samples_per_pixel = 100;  // number of rays sent into area centered at the pixel and number of samples generated for each pixel
    camera.max_depth         = 30;  // maximum number of rays bounces into scene

    camera.vfov     = 23; //  field of view angle defines the size of the viewport. Increasing fov creates zoom in effect and vice versa.
    camera.look_from = point3(13,2,3);
    camera.look_at   = point3(0,0,0);
    camera.view_up      = vec3(0,1,0);

    camera.defocus_angle = 0.6;
    camera.focus_dist    = 10.0;
    camera.render(world);

    return 0;
}