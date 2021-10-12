#pragma once

#include"hittable.h"
#include"material.h"

#include<glm/vec3.hpp>

// Stores 3 vertices, used as a storage bucket-ish
struct TriangleData {
	glm::dvec3 v0, v1, v2;
};

class Triangle : public Hittable {
public:
	// Constructors
	Triangle() {}
	Triangle(glm::dvec3 p0, glm::dvec3 p1, glm::dvec3 p2, shared_ptr<Material> m) : v0(p0), v1(p1), v2(p2), mat_ptr(m) {
		normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
	}
	Triangle(TriangleData data, glm::dvec3 rotation, shared_ptr<Material> m);

	// Virtual implementation of hit
	virtual bool hit(const Ray& ray, double t_min, double t_max, hit_record& rec) const override;

private:
	glm::dvec3 v0, v1, v2; 
	glm::dvec3 normal;
	shared_ptr<Material> mat_ptr;
};

