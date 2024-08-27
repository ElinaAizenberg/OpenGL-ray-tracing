
#ifndef PROJECT_6_VEC3_H
#define PROJECT_6_VEC3_H


class vec3{
public:
    // double has greater precision and range, but is twice the size compared to float.
    double e[3];

    vec3() : e{0,0,0} {};
    vec3(double e0, double e1, double e2) : e{e0, e1, e2} {};

    double x() const { return e[0]; }
    double y() const { return e[1]; }
    double z() const { return e[2]; }

    vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
    double operator[](int i) const { return e[i]; }
    double& operator[](int i) { return e[i]; }

    vec3& operator+=(const vec3& vec){
        e[0] += vec.e[0];
        e[1] += vec.e[1];
        e[2] += vec.e[2];
        return *this;
    }

    vec3& operator*=(double t) {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    vec3& operator/=(double t) {
        return *this *= 1/t;
    }

    double length() const
    /** The length of a vector is the square root of the sum of the squares of the vector's components. */
    {
        return std::sqrt(length_squared());
    }

    double length_squared() const
    /** Sum of the squares of the vector's components to calculate vector's length. */
    {
        return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
    }

    static vec3 random()
    /** Returns a random vector with components are in [0, 1). */
    {
        return {random_double(), random_double(), random_double()};
    }

    static vec3 random(double min, double max)
    /** Returns a random vector with components are in [min, max). */
    {
        return {random_double(min,max), random_double(min,max), random_double(min,max)};
    }

    bool near_zero() const
    /** Return true if the vector is close to zero in all dimensions. */
    {
        auto s = 1e-8;
        return (std::fabs(e[0]) < s) && (std::fabs(e[1]) < s) && (std::fabs(e[2]) < s);
    }
};

// point3 is an alias for vec3, it does not create a new type.
// alias provides an alternative name for an existing type, essentially creating a type synonym.
using point3 = vec3;


// Vector Utility Functions
/** 1. Utility functions are declared in a header file, they need to be marked as inline to avoid
 * multiple definition errors when the header is included in multiple source files.
 * 2. Inlining small utility functions reduces the function call overhead. */

inline std::ostream& operator<<(std::ostream& out, const vec3& v) {
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline vec3 operator+(const vec3& u, const vec3& v) {
    return {u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]};
}

inline vec3 operator-(const vec3& u, const vec3& v) {
    return {u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]};
}

inline vec3 operator*(const vec3& u, const vec3& v) {
    return {u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]};
}

inline vec3 operator*(double t, const vec3& v) {
    return {t*v.e[0], t*v.e[1], t*v.e[2]};
}

inline vec3 operator*(const vec3& v, double t) {
    return t * v;
}

inline vec3 operator/(const vec3& v, double t) {
    return (1/t) * v;
}

inline double dot(const vec3& u, const vec3& v)
/** The dot product of two vectors is a scalar value that measures the extent to which the two vectors are pointing in the same direction.
 * In geometric terms: a⋅b =∣a∣ * ∣b∣ * cos(θ)
 * If the dot product is positive, the vectors point in a similar direction.
 * If it is zero, the vectors are perpendicular.
 * If it is negative, the vectors point in opposite directions. */
{
    return u.e[0] * v.e[0]
           + u.e[1] * v.e[1]
           + u.e[2] * v.e[2];
}

inline vec3 cross(const vec3& u, const vec3& v)
/** The cross product of two vectors results in a new vector that is orthogonal to both of the original vectors.
 * It is commonly used in 3D space to find a vector that is normal to a surface defined by the two original vectors.
 * Mathematically, the magnitude of the cross product: ∣a×b∣= ∣a∣ * ∣b∣ * sin(θ). */
{
    return {u.e[1] * v.e[2] - u.e[2] * v.e[1],
            u.e[2] * v.e[0] - u.e[0] * v.e[2],
            u.e[0] * v.e[1] - u.e[1] * v.e[0]};
}

inline vec3 unit_vector(const vec3& v)
/**  A unit vector is one whose magnitude is equal to one.
 * To find a unit vector with the same direction as a given vector, we divide the vector by its magnitude. */
{
    return v / v.length();
}

inline vec3 random_in_unit_sphere()
/** Generates a random vector inside a unit sphere (radius of 1).
 * std::rand() used in vec3::random generates random integers in a uniform distribution over its range. */
{
    while (true) {
        auto p = vec3::random(-1,1);
        if (p.length_squared() < 1)
            return p;
    }
}

inline vec3 random_unit_vector()
/** Generates a random unit vector by normalizing a randomly generated vector inside the unit sphere. */
{
    return unit_vector(random_in_unit_sphere());
}

inline vec3 random_on_hemisphere(const vec3& normal)
/** Generates a random unit vector in the same hemisphere as the normal. */
{
    vec3 on_unit_sphere = random_unit_vector();
    // define if random vector is in the same hemisphere (in the same direction) as the normal
    if (dot(on_unit_sphere, normal) > 0.0)
        return on_unit_sphere;
    else
        return -on_unit_sphere;
}

inline vec3 random_in_unit_disk() {
    while (true) {
        auto p = vec3(random_double(-1,1), random_double(-1,1), 0);
        if (p.length_squared() < 1)
            return p;
    }
}

inline vec3 reflect(const vec3& v, const vec3& n) {
    return v - 2*dot(v,n)*n;
}

inline vec3 refract(const vec3& uv, const vec3& n, double etai_over_etat) {
    auto cos_theta = std::fmin(dot(-uv, n), 1.0);
    vec3 r_out_perp =  etai_over_etat * (uv + cos_theta*n);
    vec3 r_out_parallel = -std::sqrt(std::fabs(1.0 - r_out_perp.length_squared())) * n;
    return r_out_perp + r_out_parallel;
}




#endif //PROJECT_6_VEC3_H
