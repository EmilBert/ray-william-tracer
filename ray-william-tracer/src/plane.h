#pragma once

#include"hittable.h"
#include<glm/vec3.hpp>

class Plane : public Hittable {
public:
	Plane(const glm::dvec3& position, const glm::dvec3& normal, std::shared_ptr<Material> m) : 
		p0(position), n(glm::normalize(normal)), mat_ptr(m) {}	

	virtual bool hit(const Ray& ray, double t_min, double t_max, hit_record& rec) const override;
	virtual glm::dvec2 getUV(const glm::dvec3& p);

private:
	glm::dvec3 p0, n; // Position and normal for plane
	std::shared_ptr<Material> mat_ptr;
};