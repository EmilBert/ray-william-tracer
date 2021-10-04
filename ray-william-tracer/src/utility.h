#pragma once

#include <cmath>
#include <limits>
#include <memory>
#include<glm/vec3.hpp>
#include<glm/geometric.hpp>
#include<glm/gtx/transform.hpp>
#include<glm/gtc/quaternion.hpp>
#include<glm/gtx/quaternion.hpp>


// Usenings
using std::shared_ptr;
using std::make_shared;
using std::sqrt;

// Constants
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions

inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

inline double random_double() {
    // Returns a random real in [0,1)
    return rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max) {
    return min + (max - min) * random_double();
}

inline double clamp(double x, double min, double max) {
    // One-line pog moments
    return x <= min ? min : (x >= max ? max : x);
}

inline glm::dvec3 random_vector() {
    return { random_double(), random_double(), random_double() };
}

inline glm::dvec3 random_in_unit_sphere() {
    while (true) {
        glm::dvec3 p = random_vector();
        if (glm::dot(p, p) >= 1) continue; // We are outside unit-sphere
        return p;
    }
}

inline glm::dvec3 random_in_hemisphere(const glm::dvec3& normal) {
    glm::dvec3 in_unit_sphere = random_in_unit_sphere();
    if (dot(in_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
        return in_unit_sphere;
    else
        return -in_unit_sphere;
}

inline glm::dvec3 random_unit_vector() {
    return glm::normalize(random_in_unit_sphere());
}

inline bool isVectorNearZero(const glm::dvec3& vec) {
    const auto s = 1e-8;
    return (fabs(vec.x) < s) && (fabs(vec.y) < s) && (fabs(vec.z) < s);
}

// Common Headers

#include "ray.h"
