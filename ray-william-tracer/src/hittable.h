#pragma once

#include"ray.h"
#include"hit_record.h"
#include<glm/geometric.hpp>
#include<glm/vec3.hpp>
#include<glm/vec2.hpp>

class Texture;
class Material;
// Abstract class

class Hittable {
public:
	// Virtual methods
	virtual bool hit(const Ray& ray, double t_min, double t_max, hit_record& rec) const = 0;

	virtual bool isLight() { return false; }

	// Each hittable can override this to describe its UV at a given point on its surface
	virtual glm::dvec2 getUV(const glm::dvec3& p) { return { 0.0, 0.0 }; }

	// Each hittable has the ability to have a texture attached to it
	virtual Texture* getTexture() const { return nullptr; }

};