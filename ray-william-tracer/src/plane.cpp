#include"plane.h"

#include<glm/vec3.hpp>
#include<iostream>

bool Plane::hit(const Ray& ray, double t_min, double t_max, hit_record& rec) const
{
    auto denom = glm::dot(n, ray.direction());
    if (glm::abs(denom) > 1e-6) {
        glm::dvec3 toRay = p0 - ray.origin();
        double t = glm::dot(toRay, n) / denom;

        // Set some stuff
        rec.set_face_normal(ray, n);
        rec.t = t;
        rec.p = ray.orig + ray.dir * t;
        rec.mat_ptr = mat_ptr;

        if (t <= t_min || t >= t_max)
            return false;

        return t >= 0;
    }

    return false;
}

glm::dvec2 Plane::getUV(const glm::dvec3& p)
{
    return { 0,0 };
}

Texture* Plane::getTexture() const
{
    return texture_ptr.get();
}
