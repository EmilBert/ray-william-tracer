#pragma once

#include"ray.h"
#include"hit_record.h"
#include<glm/geometric.hpp>
#include<glm/vec3.hpp>
#include<glm/vec2.hpp>

class Material;
// Abstract class

class Hittable {
public:
	// Virtual methods
	virtual bool hit(const Ray& ray, double t_min, double t_max, hit_record& rec) const = 0;

	virtual bool isLight() { return false; }

	// Each object handles UV differently
	virtual glm::dvec2 getUV(const glm::dvec3& p) { return glm::dvec2(0, 0); }
};