
#ifndef PROJECT_6_HITTABLE_H
#define PROJECT_6_HITTABLE_H

#include "common.h"

class material;

class hit_record {
public:
    point3 point;
    vec3 normal;
    shared_ptr<material> hit_material;
    double t;
    bool front_face;


    void set_face_normal(const ray& ray, const vec3& outward_normal)
    /** Defines if the ray intersects the object from the outside or inside. If the ray is outside the object
     * (the ray and the normal face in the opposite directions), the normal will point outward,
     * but if the ray is inside the sphere (the ray and the normal face in the same directions), the normal will point inward.
     * In the last case, we reverse the direction of the normal, to make normals always point against the incident ray.
     */
    {
        // NOTE: the parameter `outward_normal` is assumed to have unit length.
        front_face = dot(ray.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable {
public:
    virtual ~hittable() = default;

    // the hit only “counts” if t(min)< t < t(max)
    virtual bool hit(const ray& ray, interval ray_t_interval, hit_record& record) const = 0;
};


#endif //PROJECT_6_HITTABLE_H
