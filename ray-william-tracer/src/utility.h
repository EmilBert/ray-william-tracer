#pragma once

#include <cmath>
#include <limits>
#include <memory>
#include<iostream>
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
    double x = rand() / (RAND_MAX + 1.0);
    return x;
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

inline glm::dvec3 random_vector(double min, double max) {
    return { random_double(min,max), random_double(min,max), random_double(min,max) };
}

inline glm::dvec3 random_in_unit_sphere() {
    int i = 0;
    while (true) {
        if (i > 0) std::cerr << "Running random_in_unit_sphere: " << i << std::endl;
        glm::dvec3 p = random_vector(-1, 1);
        if (glm::dot(p, p) >= 1) continue; // We are outside unit-sphere
        i++;
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
    auto x = glm::normalize(random_in_unit_sphere());
    return x;
}

inline bool isVectorNearZero(const glm::dvec3& vec) {
    const auto s = 1e-6;
    return (fabs(vec.x) < s) && (fabs(vec.y) < s) && (fabs(vec.z) < s);
}

inline void logProgress(int curr, int total) {
    double percentage = curr / static_cast<double>(total);

    //if(percentage < )
}