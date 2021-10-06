#pragma once

#include"hittable.h"
#include"material.h"
#include<glm/vec3.hpp>
#include<glm/geometric.hpp>
#include<glm/gtx/rotate_vector.hpp>

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

inline Triangle::Triangle(TriangleData data, glm::dvec3 rotation, shared_ptr<Material> m)
{
	v0 = glm::rotateX(data.v0, rotation.x);
	v1 = glm::rotateX(data.v1, rotation.x);
	v2 = glm::rotateX(data.v2, rotation.x);

	v0 = glm::rotateY(v0, rotation.y);
	v1 = glm::rotateY(v1, rotation.y);
	v2 = glm::rotateY(v2, rotation.y);

	v0 = glm::rotateZ(v0, rotation.z);
	v1 = glm::rotateZ(v1, rotation.z);
	v2 = glm::rotateZ(v2, rotation.z);

	mat_ptr = m;

	normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
}

// Algorithm used: Möller Trumbore method
bool Triangle::hit(const Ray& ray, double t_min, double t_max, hit_record& rec) const
{
	// Do some math to check if incoming ray intersects triangle between t_min and t_max 
	// then place some data into rec
	
	glm::dvec3 v0v1 = v1 - v0;
	glm::dvec3 v0v2 = v2 - v0;
	glm::dvec3 pvec = glm::cross(ray.dir, v0v2);
	double det = glm::dot(v0v1, pvec);

	constexpr double eps = 0.001;

	// If the determinant is negative the triangle is backfacing
	// if the determinant is close to 0, the ray misses the triangle
	// uncomment this re-assignment to "disable" back face culling
	if (det > -eps && det < eps) return false;

	float invDet = 1 / det;

	glm::dvec3 tvec = ray.orig - v0;
	double u = glm::dot(tvec, pvec) * invDet;
	if (u < 0 || u > 1) return false;

	glm::dvec3 qvec = cross(tvec, v0v1);
	double v = glm::dot(ray.dir, qvec) * invDet;
	if (v < 0 || u + v > 1) return false;

	double t = glm::dot(v0v2, qvec) * invDet;

	if (t > t_max || t < t_min) return false;

	// check if t is between t_min & t_max
	// record into rec
	// Ok collision has happend
	rec.set_face_normal(ray, normal);
	rec.p = ray.direction() * t;
	rec.t = t;
	rec.mat_ptr = mat_ptr;

	return true;
}