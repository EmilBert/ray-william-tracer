#pragma once

#include "utility.h"
#include "hittable.h"

#include<glm/geometric.hpp>

class Material {
public:

    virtual glm::dvec3 emitted(const glm::dvec3& p) const {
        return glm::dvec3(0, 0, 0);
    }

    virtual bool scatter(const Ray& ray_in, const hit_record& rec, glm::dvec3& attenuation, Ray& scattered
    ) const = 0;
};

class Unlit : public Material {
public:
    Unlit(const glm::dvec3& c) : color(c) {}

    virtual bool scatter(
        const Ray& r_in, const hit_record& rec, glm::dvec3& attenuation, Ray& scattered
    ) const override {
        attenuation = color;
        return true;
    }

public:
    glm::dvec3 color;
};

class Metal : public Material {
public:
    Metal(const glm::dvec3& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

    virtual bool scatter(
        const Ray& r_in, const hit_record& rec, glm::dvec3& attenuation, Ray& scattered
        ) const override {
        glm::dvec3 refelected = glm::reflect(glm::normalize(r_in.direction()), rec.normal);
        scattered = Ray(rec.p, refelected + fuzz*random_in_unit_sphere());
        attenuation = albedo;
        return (glm::dot(scattered.direction(), rec.normal) > 0);
    }

public:
    glm::dvec3 albedo;
    double fuzz;
};

class Lambertian :  public Material {
public:
    Lambertian(const glm::dvec3& a) : albedo(a) {}

    virtual bool scatter(
        const Ray& ray_in, const hit_record& rec, glm::dvec3& attentuation, Ray& scattered
    ) const override {

        // TODO: Russian roulette implementation (see lecture 10 notes)

        auto scatter_direction = rec.normal + random_unit_vector();
        
        //Catch degenrate scatter direction
        if(isVectorNearZero(scatter_direction))
            scatter_direction = rec.normal;
        
        scattered = Ray(rec.p, scatter_direction);
        attentuation = albedo;
        return true;
    }

public:
    glm::dvec3 albedo;
};

class Dielectric : public Material {
public:
    Dielectric(double index_of_refraction) : refraction_index(index_of_refraction) {}

    virtual bool scatter(
        const Ray& ray_in, const hit_record& rec, glm::dvec3& attentuation, Ray& scattered
    ) const override {
        attentuation = glm::dvec3(1.0, 1.0, 1.0);
        double refraction_ratio = rec.front_face ? (1.0 / refraction_index) : refraction_index;
        glm::dvec3 unit_direction = glm::normalize(ray_in.direction());
        glm::dvec3 refracted = glm::refract(unit_direction, rec.normal, refraction_ratio);

        double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = sqrt(1.0 - cos_theta * cos_theta); // Trig-ettan 
        bool cannot_refract = refraction_ratio * sin_theta > 1.0;
        glm::dvec3 direction;

        // This is some weird hack to simulate how glass looks at an angle
        if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double())
            direction = glm::reflect(unit_direction, rec.normal);
        else
            direction = glm::refract(unit_direction, rec.normal, refraction_ratio);

        scattered = Ray(rec.p, refracted);
        return true;
    }

public:
    double refraction_index;

private:
    static double reflectance(double cosine, double refraction_index) {
        // Use Schlick's approximation for reflectance
        double r0 = (1 - refraction_index) / (1 + refraction_index);
        r0 = r0 * r0;
        return r0 + (1 - r0) * pow((1 - cosine), 5);
    }
};

//  Light Sorces
class Diffuse_light : public Material {
public:
    Diffuse_light(glm::dvec3 c) : emit(c) {}
    virtual bool scatter(const Ray& ray_in, const hit_record& rec, glm::dvec3& attenuation, Ray& scattered
    ) const override {
        return false;
    }
    
    virtual glm::dvec3 emitted(const glm::dvec3& p) const override {
        return emit;
    }

public:
    glm::dvec3 emit;
};