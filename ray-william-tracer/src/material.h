#pragma once

#include "utility.h"
#include "hittable.h"

class Light;

#include<glm/geometric.hpp>

class Material {
public:
    virtual glm::dvec3 emitted(const glm::dvec3& p) const {
        return glm::dvec3(0, 0, 0);
    }

    virtual bool scatter(const Ray& ray_in, const hit_record& rec, glm::dvec3& attenuation, Ray& scattered
    ) const = 0;

    virtual glm::dvec3 light_pass(const glm::dvec3& light_pos, const hit_record& rec, Light* l) { return glm::dvec3(0, 0, 0); }
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
    ) const override;

    virtual glm::dvec3 light_pass(const glm::dvec3& light_pos, const hit_record& rec, Light* l);

public:
    glm::dvec3 albedo;
    double fuzz;
};

class Lambertian :  public Material {
public:
    Lambertian(const glm::dvec3& a) : albedo(a) {}

    virtual bool scatter(
        const Ray& ray_in, const hit_record& rec, glm::dvec3& attentuation, Ray& scattered
    ) const override;

    virtual glm::dvec3 light_pass(const glm::dvec3& light_pos, const hit_record& rec, Light* l);

public:
    glm::dvec3 albedo;
};

class Dielectric : public Material {
public:
    Dielectric(double index_of_refraction) : refraction_index(index_of_refraction) {}

    virtual bool scatter(
        const Ray& ray_in, const hit_record& rec, glm::dvec3& attentuation, Ray& scattered
    ) const override;

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