#pragma once

#include"ray.h"
#include"hit_record.h"
#include<glm/geometric.hpp>

class Material;
// Abstract class

class Hittable {
	public:
		virtual bool hit(const Ray& ray, double t_min, double t_max, hit_record& rec) const = 0;
		//virtual bool bounding_box(double time0, double time1, aabb& output_box) const { return false; }
};