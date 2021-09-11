#pragma once

#include "ray.h"
#include "utility.h"

class Material;
// Abstract class

struct hit_record {
	Point3 p;
	Vec3 normal;
	shared_ptr<Material> mat_ptr;
	double t;
	bool front_face;

	inline void set_face_normal(const Ray& ray, const Vec3& outward_normal) {
		front_face = dot(ray.direction(), outward_normal) < 0;
		normal = front_face ? outward_normal : -outward_normal;
	}
};

class Hittable {
	public:
		virtual bool hit(const Ray& ray, double t_min, double t_max, hit_record& rec) const = 0;
};