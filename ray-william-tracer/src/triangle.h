#pragma once

#include"hittable.h"
#include"vec3.h"

class Triangle : public Hittable {
public:
	Triangle(){}
	Triangle(Point3 p0, Point3 p1, Point3 p2) : v0(p0), v1(p1), v2(p2) {}

	virtual bool hit(const Ray& ray, double t_min, double t_max, hit_record& rec) const override;

private:
	Point3 v0, v1, v2; // Triangle vertices
};


bool Triangle::hit(const Ray& ray, double t_min, double t_max, hit_record& rec) const
{
	// Do some math to check if incoming ray intersects triangle between t_min and t_max 
	// then place some data into rec
	
	Vec3 v0v1 = v1 - v0;
	Vec3 v0v2 = v2 - v0;
	Vec3 pvec = cross(ray.dir, v0v2);
	double det = dot(v0v1, pvec);

	constexpr double eps = 0.00001;


	// If the determinant is negative the triangle is backfacing
	// if the determinant is close to 0, the ray misses the triangle
	// uncomment this re-assignment to "disable" back face culling
	det = fabs(det);
	if (det < eps) return false;
	
	float invDet = 1 / det;

	Vec3 tvec = ray.orig - v0;
	double u = dot(tvec, pvec) * invDet;
	if (u < 0 || u > 1) return false;

	Vec3 qvec = cross(tvec, v0v1);
	double v = dot(ray.dir, qvec) * invDet;
	if (v < 0 | u + v > 1) return false;

	double t = dot(v0v2, qvec) * invDet;

	return true;
}