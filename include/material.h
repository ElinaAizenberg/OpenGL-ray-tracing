
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
    color albedo; // albedo - Latin for “whiteness”
};

class metal : public material {
public:
    metal(const color& albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered)
    const override
    {
        // calculate the direction of the reflected ray using formula:  r=v−2⋅dot(v,n)⋅n
        vec3 reflected = reflect(r_in.direction(), rec.normal);

        // Fuzziness is created by adding a small sphere to the reflected ray,
        // scaled by fuzz parameter and thus randomizing the direction of the reflected ray.
        // Fuzz sphere needs to be consistently scaled compared to the reflection vector, which requires to normalize reflected ray
        reflected = unit_vector(reflected) + (fuzz * random_unit_vector());

        scattered = ray(rec.point, reflected);
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }

private:
    color albedo;
    double fuzz;
};

/** When a light ray hits dielectric material, it splits into a reflected ray and a refracted (transmitted) ray.
 * A reflected ray hits a surface and then “bounces” off in a new direction.
 * A refracted ray bends as it transitions from a material's surroundings into the material itself.
 * The amount that a refracted ray bends is determined by the material's refractive index.
 */

class dielectric : public material {
public:
    dielectric(double refraction_index) : refraction_index(refraction_index) {}

    bool scatter(const ray& ray_in, const hit_record& rec, color& attenuation, ray& scattered) const override
    {
        attenuation = color(1.0, 1.0, 1.0); // the material does not absorb any color
        // stores the ratio of the refraction indices depending on whether the ray is entering (front_face)
        // or exiting the material. If it's entering, the ratio is 1.0 / refraction_index.
        double ri = rec.front_face ? (1.0/refraction_index) : refraction_index;

        vec3 unit_direction = unit_vector(ray_in.direction());
        // the cosine of the angle between the incoming ray and the surface normal
        double cos_theta = std::fmin(dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = std::sqrt(1.0 - cos_theta*cos_theta);

        // checks if total internal reflection happens.
        // Total internal reflection occurs when the angle is too steep for the light to pass through the material.
        // If this condition is true, the light will reflect instead of refract.
        bool cannot_refract = ri * sin_theta > 1.0;
        vec3 direction;

        if (cannot_refract || reflectance(cos_theta, ri) > random_double())
            // if refraction cannot happen, or if the random number is greater than the calculated reflectance,
            // the light reflects.
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
    static double reflectance(double cosine, double refraction_index)
    /** Calculate the reflectance of light at a surface. */
    {
        /** Use Schlick's approximation: the reflectance of light depends on both the material's refractive index
         * and the angle at which the light hits the surface.
         */
        // the reflectance at normal incidence (i.e., when the light hits the surface straight on, at a 90-degree angle).
        auto r0 = (1 - refraction_index) / (1 + refraction_index);
        r0 = r0*r0;
        // The formula estimates how reflectance changes as the angle of incidence
        // (the angle between the incoming light and the surface normal) increases.
        return r0 + (1-r0)*std::pow((1 - cosine),5);
    }
};

#endif //PROJECT_6_MATERIAL_H
