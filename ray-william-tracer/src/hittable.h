#pragma once

#include "ray.h"
#include "utility.h"
#include<glm/geometric.hpp>

class Material;
// Abstract class

struct hit_record {
	glm::dvec3 p;
	glm::dvec3 normal;
	shared_ptr<Material> mat_ptr;
	double t;
	bool front_face;

	inline void set_face_normal(const Ray& ray, const glm::dvec3& outward_normal) {
		front_face = glm::dot(ray.direction(), outward_normal) < 0;
		normal = front_face ? outward_normal : -outward_normal;
	}


};

class Hittable {
	public:
		virtual bool hit(const Ray& ray, double t_min, double t_max, hit_record& rec) const = 0;
		//virtual bool bounding_box(double time0, double time1, aabb& output_box) const { return false; }
};