#pragma once

#include"hittable.h"
#include<glm/vec3.hpp>

#include"texture.h"

class Plane : public Hittable {
public:
	Plane(const glm::dvec3& position, const glm::dvec3& normal, std::shared_ptr<Material> m) : 
		p0(position), n(glm::normalize(normal)), mat_ptr(m) {}	
	
	// Texture constructor
	Plane(const glm::dvec3& position, const glm::dvec3& normal, std::shared_ptr<Material> m, std::shared_ptr<Texture> t) : 
		p0(position), n(glm::normalize(normal)), mat_ptr(m), texture_ptr(t) {}

	virtual bool hit(const Ray& ray, double t_min, double t_max, hit_record& rec) const override;
	virtual glm::dvec2 getUV(const glm::dvec3& p);
	virtual Texture* getTexture() const;

private:
	glm::dvec3 p0, n; // Position and normal for plane
	std::shared_ptr<Material> mat_ptr;
	std::shared_ptr<Texture> texture_ptr;
};