#pragma once

#include "hittable.h"
#include "vec3.h"

class Sphere : public Hittable {
	public:
		Sphere() {}
		Sphere(Point3 cen, double r, shared_ptr<Material> m) : center(cen), radius(r), mat_ptr(m) {};

		virtual bool hit(const Ray& ray, double t_min, double t_max, hit_record& rec) const override;

	public:
		Point3 center;
		double radius;
		shared_ptr<Material> mat_ptr;
};

// KNOCKOUT!
bool Sphere::hit(const Ray& ray, double t_min, double t_max, hit_record& rec) const {
	Vec3 oc = ray.origin() - center; // Ray pointing from ray origin to center
	double a = ray.direction().length_squared(); // Length of the ray squared

	// Some constants to describe wheter our ray intersects the sphere or not
	double half_b = dot(oc, ray.direction());
	double c = oc.length_squared() - radius * radius;

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
	
	Vec3 outward_normal = (rec.p - center) / radius;
	rec.set_face_normal(ray, outward_normal);
	rec.mat_ptr = mat_ptr;

	return true;

}