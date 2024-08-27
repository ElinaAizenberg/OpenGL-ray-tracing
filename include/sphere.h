
#ifndef PROJECT_6_SPHERE_H
#define PROJECT_6_SPHERE_H

#include "hittable.h"


class sphere : public hittable {
public:
    sphere(const point3& center, double radius, shared_ptr<material> mat)
            : center(center), radius(std::fmax(0,radius)), sphere_material(mat) {}


    /** We want to know if our ray P(t)=Q+td ever hits the sphere anywhere.
     * If it does hit the sphere, there is some t for which P(t)  satisfies the sphere equation (Cx−x)^2+(Cy−y)^2+(Cz−z)^2=r^2.
     * So we are looking for any t where this is true: (C−P(t))⋅(C−P(t))=r^2 because
     * the vector from point P=(x,y,z) to sphere center C=(Cx,Cy,Cz) is (C−P).
     * */
    bool hit(const ray& ray, interval ray_t_interval, hit_record& record) const override
    {
        // Reminder: a ray is a function P(t)=A+tb, where Q - the ray origin, d - the ray direction.
        vec3 oc = center - ray.origin(); // <- (C-Q)
        // a = d*d; length_squared is this case is the same as dot product of the vector to itself
        auto a = ray.direction().length_squared();
        // b = -2d * (C-Q); if b = -2h -> h = d * (C-Q)
        auto h = dot(ray.direction(), oc);
        // c = (C-Q)*(C-Q) - r^2
        auto c = oc.length_squared() - radius*radius;

        auto discriminant = h*h - a*c;
        if (discriminant < 0)
            return false;

        auto sqrtd = std::sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        auto root = (h - sqrtd) / a; // first possible root

        // check if the root falls into the interval from the closest (min = 0.001) point to the closest at the moment of hitting
        // the maximum or closest value of the interval is updated in the hittable_list IF the ray hits any object.
        if (!ray_t_interval.surrounds(root))
        {
            root = (h + sqrtd) / a;

            if (!ray_t_interval.surrounds(root))
                return false;
        }

        record.t = root; // the ray parameter t in ray function definition P(t)=Q+td
        record.point = ray.at(root);
        record.hit_material = sphere_material;
        vec3 outward_normal = (record.point - center) / radius; // unit length normal at the hit point
        record.set_face_normal(ray, outward_normal); // define whether ray intersects from inside/outside the sphere and

        return true;
    }

private:
    point3 center;
    double radius;
    shared_ptr<material> sphere_material;
};



#endif //PROJECT_6_SPHERE_H
