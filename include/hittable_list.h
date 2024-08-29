#ifndef PROJECT_6_HITTABLE_LIST_H
#define PROJECT_6_HITTABLE_LIST_H

#include "common.h"
#include "hittable.h"
#include <vector>



class hittable_list : public hittable {
public:
    /** shared_ptr<type> is a pointer with reference-counting semantics.
     * Every time you assign its value to another shared pointer, the reference count is incremented.
     * As shared pointers go out of scope , the reference count is decremented.
     * Once the count goes to zero, the object is safely deleted.
     */
    std::vector<shared_ptr<hittable>> objects;

    hittable_list() = default;
    explicit hittable_list(shared_ptr<hittable> object) { add(object); }

    void clear() { objects.clear(); }

    void add(shared_ptr<hittable> object)
    {
        objects.push_back(object);
    }

    bool hit(const ray& ray, interval ray_t_interval, hit_record& record) const override
    /** Iterate over a vector of objects' pointers and update hit record instance if the ray hits any objects in the list. */
    {
        hit_record temp_record;
        bool hit_anything = false;
        auto closest_so_far = ray_t_interval.max; // initial value is infinity

        for (const auto& object : objects)
        {
            if (object->hit(ray, interval(ray_t_interval.min, closest_so_far), temp_record))
            {
                hit_anything = true;
                closest_so_far = temp_record.t;
                record = temp_record;
            }
        }

        return hit_anything;
    }
};

#endif //PROJECT_6_HITTABLE_LIST_H
