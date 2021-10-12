#pragma once

#include<glm/vec3.hpp>
#include<vector>

#include"triangle.h"

class Light : public Hittable {
public:
	// Assume non-skewed quads only!
	Light(std::vector<glm::dvec3> vertices, glm::dvec3 p); 

	bool hit(const Ray& ray, double t_min, double t_max, hit_record& rec) const override;
	bool isLight() override { return true; }

	const std::vector<glm::dvec3>& get_sample_points() { return sample_points; }
	glm::dvec3 position;

private:
	std::vector<glm::dvec3> sample_points; // Points ON the quad
	Triangle t0,t1;
};