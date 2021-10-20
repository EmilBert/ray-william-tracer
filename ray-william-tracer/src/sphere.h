#pragma once

#include "hittable.h"
#include<glm/vec3.hpp>
#include<memory>

#include"texture.h"


class Sphere : public Hittable {
	public:
		Sphere() {}
		Sphere(glm::dvec3 cen, double r, std::shared_ptr<Material> m) : center(cen), radius(r), mat_ptr(m) {};
		Sphere(glm::dvec3 cen, double r, std::shared_ptr<Material> m, std::shared_ptr<Texture> t) : center(cen), radius(r), mat_ptr(m), texture_ptr(t) {};

		virtual bool hit(const Ray& ray, double t_min, double t_max, hit_record& rec) const override;
		virtual glm::dvec2 getUV(const glm::dvec3& p);
		virtual Texture* getTexture() const;

	public:
		glm::dvec3 center;
		double radius;
		std::shared_ptr<Material> mat_ptr;
		std::shared_ptr<Texture> texture_ptr;
};

