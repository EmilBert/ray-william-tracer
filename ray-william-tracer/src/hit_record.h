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

	void set_face_normal(const Ray& ray, const glm::dvec3& outward_normal);

};