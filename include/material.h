
#ifndef PROJECT_6_MATERIAL_H
#define PROJECT_6_MATERIAL_H

#include "common.h"

// forward declaration.
class hit_record;

class material {
public:
    virtual ~material() = default;

    virtual bool scatter(const ray& ray_in, const hit_record& record, color& attenuation, ray& scattered) const
    {
        return false;
    }
};

/** Diffuse material
 * 1. Light that reflects off a diffuse surface has its direction randomized.
 * 2. A ray that hits the surface of diffuse material has an equal probability of bouncing in any direction away from the surface.
 * 3. The darker the surface, the more likely the ray is absorbed.
 * 4. If a ray bounces off of a material and keeps 100% of its color, then we say that the material is white.
 * 5. If a ray bounces off of a material and keeps 0% of its color, then we say that the material is black.
 */

/** Lambertian distribution scatters reflected rays in a manner that is proportional to cos(ϕ),
 * where ϕ is the angle between the reflected ray and the surface normal.
 * A reflected ray is most likely to scatter in a direction near the surface normal,
 * and less likely to scatter in directions away from the normal.
 *
 * Shadows: more light bounces straight-up, so the area underneath the sphere is darker.
*/
class lambertian : public material {
public:
    explicit lambertian(const color& albedo) : albedo(albedo) {}

    bool scatter(const ray& ray_in, const hit_record& record, color& attenuation, ray& scattered) const override
    {
        // create Lambertian distribution by adding a random unit vector to the normal vector
        auto scatter_direction = record.normal + random_unit_vector();
        scattered = ray(record.point, scatter_direction);

        // Catch degenerate scatter direction
        if (scatter_direction.near_zero())
            scatter_direction = record.normal;

        attenuation = albedo;
        return true;
    }

private:
    color albedo;
};

class metal : public material {
public:
    metal(const color& albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered)
    const override {
        vec3 reflected = reflect(r_in.direction(), rec.normal);
        reflected = unit_vector(reflected) + (fuzz * random_unit_vector());

        scattered = ray(rec.point, reflected);
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }

private:
    color albedo;
    double fuzz;
};

class dielectric : public material {
public:
    dielectric(double refraction_index) : refraction_index(refraction_index) {}

    bool scatter(const ray& ray_in, const hit_record& rec, color& attenuation, ray& scattered)
    const override {
        attenuation = color(1.0, 1.0, 1.0);
        double ri = rec.front_face ? (1.0/refraction_index) : refraction_index;

        vec3 unit_direction = unit_vector(ray_in.direction());
        double cos_theta = std::fmin(dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = std::sqrt(1.0 - cos_theta*cos_theta);

        bool cannot_refract = ri * sin_theta > 1.0;
        vec3 direction;

        if (cannot_refract || reflectance(cos_theta, ri) > random_double())
            direction = reflect(unit_direction, rec.normal);
        else
            direction = refract(unit_direction, rec.normal, ri);

        scattered = ray(rec.point, direction);
        return true;
    }

private:
    // Refractive index in vacuum or air, or the ratio of the material's refractive index over
    // the refractive index of the enclosing media
    double refraction_index;
    static double reflectance(double cosine, double refraction_index) {
        // Use Schlick's approximation for reflectance.
        auto r0 = (1 - refraction_index) / (1 + refraction_index);
        r0 = r0*r0;
        return r0 + (1-r0)*std::pow((1 - cosine),5);
    }
};

#endif //PROJECT_6_MATERIAL_H
