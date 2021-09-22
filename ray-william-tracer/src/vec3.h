#pragma once

#include <cmath>
#include <iostream>

#include"utility.h"

class Vec3 {
public:
	// constructors
	Vec3() : e{ 0,0,0 } {}
	Vec3(double e0, double e1, double e2) : e{ e0, e1, e2 } {}
	
	// acessing the axis
	double x() const { return e[0]; }
	double y() const { return e[1]; }
	double z() const { return e[2]; }
	

	// Reflection related
	bool near_zero() const {
		//Return true if the vector is close to zero in alla dimensions
		const auto s = 1e-8;
		return (fabs(e[0]) < s) && (fabs(e[1]) < s) && (fabs(e[2]) < s);
	}

	// math operators
	Vec3 operator-() const { return Vec3(-e[0], -e[1], -e[2]); }
	double operator[](int i) const { return e[i]; }
	double& operator[](int i) { return e[i]; }

	// addition between vectors
	Vec3& operator+=(const Vec3& v) {
		e[0] += v.e[0];
		e[1] += v.e[1];
		e[2] += v.e[2];
		return *this;
	}

	// vector scalar multiplication 
	Vec3& operator*=(const double t) {
		e[0] *= t;
		e[1] *= t;
		e[2] *= t;
		return *this;
	}

	// vector scalar division
	Vec3& operator/=(const double t) {
		return *this *= 1 / t;
	}

	// get the non-squared length
	double length() const {
		return std::sqrt(length_squared());
	}

	// get the squared length
	double length_squared() const {
		return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
	}

	// Random point inside unit cube
	inline static Vec3 random(){
		return Vec3(random_double(), random_double(), random_double());
	}

	inline static Vec3 random(double min, double max){
		return Vec3(random_double(min,max), random_double(min,max), random_double(min,max));
	}

	
public:
	double e[3];

};

// Type aliasing
using Point3 = Vec3;
using Color = Vec3;

// Here comes a bunch of utility functions for our Vec3 class

// Print vector, for debugging etc
inline std::ostream& operator<<(std::ostream& out, const Vec3& v) {
	return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

// addition where order doesn't matter 
inline Vec3 operator+(const Vec3& u, const Vec3& v) {
	return Vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

// subtraction where order doesn't matter
inline Vec3 operator-(const Vec3& u, const Vec3& v) {
	return Vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

// element-wise multiplication between vectors
inline Vec3 operator*(const Vec3& u, const Vec3& v) {
	return Vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

// scalar multiplication & division, order doesn't matter
inline Vec3 operator*(double t, const Vec3& v) { return Vec3(t * v.e[0], t * v.e[1], t * v.e[2]); }
inline Vec3 operator*(const Vec3& v, double t) { return t * v; }
inline Vec3 operator/(Vec3 v, double t) { return (1 / t) * v; }

// The dot product between two vectors returns a scalar
// it goes as: v1.x * v2.x + v1.y*v2.y + v1.z*v2.z
inline double dot(const Vec3& u, const Vec3& v) {
	return u.e[0] * v.e[0]
		 + u.e[1] * v.e[1]
		 + u.e[2] * v.e[2];
}

// Cross product with 3d vectors returns a 3d vector3
// Use determinant between these 2 vectors + unit vectors to get formula
inline Vec3 cross(const Vec3& u, const Vec3& v) {
	return Vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
		u.e[2] * v.e[0] - u.e[0] * v.e[2],
		u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

// Return a random point inside unit vector
inline Vec3 random_in_unit_sphere(){
	while(true){
		Vec3 p = Vec3::random(-1,1);
		if(p.length_squared() >= 1) continue; // Are we outisde unit-sphere? Redo the randomness and try again
		return p;
	}
}

Vec3 random_in_hemisphere(const Vec3& normal) {
	Vec3 in_unit_sphere = random_in_unit_sphere();
	if (dot(in_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
		return in_unit_sphere;
	else
		return -in_unit_sphere;
}

// Takes a Vec3 and converts to unit vector
inline Vec3 unit_vector(Vec3 v) {
	return v / v.length();
}

inline Vec3 normalize(Vec3 v) { return unit_vector(v); }

Vec3 random_unit_vector() {
    return unit_vector(random_in_unit_sphere());
}

Vec3 reflect(const Vec3& v, const Vec3& n) {
    return v - 2*dot(v,n)*n;
}

Vec3 refract(const Vec3& uv, const Vec3& n, double etai_over_etat){
	double cos_theta = fmin(dot(-uv, n), 1.0);
	Vec3 r_out_perp = etai_over_etat*(uv + cos_theta*n);
	Vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.length_squared())) * n;
	return r_out_perp + r_out_parallel;
}
