#include"hittable_list.h"

#include"hittable.h"

#include"light.h"

void HittableList::add(shared_ptr<Hittable> object)
{
    objects.push_back(object);

    // Is it also a light?
    if (object->isLight()) {
        lights.push_back(std::dynamic_pointer_cast<Light>(object));
    }
}

bool HittableList::hit(const Ray& r, double t_min, double t_max, hit_record& rec) const {
    hit_record temp_rec;
    bool hit_anything = false;
    auto closest_so_far = t_max;

    for (const auto& object : objects) {
        if (object->hit(r, t_min, closest_so_far, temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
            rec.hittable_ptr = object;
        }
    }

    return hit_anything;
}

