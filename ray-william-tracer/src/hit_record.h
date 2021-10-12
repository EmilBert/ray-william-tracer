#pragma once

#include<glm/vec3.hpp>
#include<glm/geometric.hpp>
#include<memory>

#include"ray.h"

class Material;
class Hittable;

struct hit_record {
	glm::dvec3 p;
	glm::dvec3 normal;
	std::shared_ptr<Material> mat_ptr;
	double t;
	bool front_face;
	std::shared_ptr<Hittable> hittable_ptr;

	inline void set_face_normal(const Ray& ray, const glm::dvec3& outward_normal) {
		front_face = glm::dot(ray.direction(), outward_normal) < 0;
		normal = front_face ? outward_normal : -outward_normal;
	}


};