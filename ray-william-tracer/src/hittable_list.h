#pragma once

#include <memory>
#include <vector>

#include"hittable.h"

using std::shared_ptr;
using std::make_shared;

class HittableList : public Hittable {
public:
    HittableList() {}
    HittableList(shared_ptr<Hittable> object) { add(object); }

    void clear() { objects.clear(); }
    void add(shared_ptr<Hittable> object) { 
        objects.push_back(object); 
        if ((*object).isLight()) light_indices.push_back(objects.size() - 1);
    }

    virtual bool hit(
        const Ray& r, double t_min, double t_max, hit_record& rec) const override;

public:
    std::vector<shared_ptr<Hittable>> objects;
    std::vector<int> light_indices;
};

