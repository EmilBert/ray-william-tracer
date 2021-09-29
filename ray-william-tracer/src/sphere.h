#pragma once

#include "hittable.h"
#include<glm/vec3.hpp>

class Sphere : public Hittable {
	public:
		Sphere() {}
		Sphere(glm::dvec3 cen, double r, shared_ptr<Material> m) : center(cen), radius(r), mat_ptr(m) {};

		virtual bool hit(const Ray& ray, double t_min, double t_max, hit_record& rec) const override;

	public:
		glm::dvec3 center;
		double radius;
		shared_ptr<Material> mat_ptr;
};

// KNOCKOUT!
bool Sphere::hit(const Ray& ray, double t_min, double t_max, hit_record& rec) const {
	glm::dvec3 oc = ray.origin() - center; // Ray pointing from ray origin to center
	double a = glm::dot(ray.direction(), ray.direction()); // Length of the ray squared // @POTENTIAL: Glm convert 

	// Some constants to describe wheter our ray intersects the sphere or not
	double half_b = glm::dot(oc, ray.direction());
	double c = glm::dot(oc,oc) - radius * radius; // @POTENTIAL: Glm convert

	double discriminant = half_b * half_b - a * c;
	if (discriminant < 0) return false;
	double sqrtd = sqrt(discriminant);

	// Find the nearest root that lies in the acceptable range.
	double root = (-half_b - sqrtd) / a;
	if (root < t_min || t_max < root) {
		root = (-half_b + sqrtd) / a;
		if (root < t_min || t_max < root) return false;
	}

	rec.t = root;
	rec.p = ray.at(rec.t);
	
	glm::dvec3 outward_normal = (rec.p - center) / radius;
	rec.set_face_normal(ray, outward_normal);
	rec.mat_ptr = mat_ptr;

	return true;

}