#pragma once

#include "utility.h"
#include "hittable.h"

class Light;
class Scene;
class Texture;

class Material {
public:
    virtual glm::dvec3 emitted(const glm::dvec3& p) const {
        return glm::dvec3(0, 0, 0);
    }

    virtual bool scatter(const Ray& ray_in, const hit_record& rec, glm::dvec3& attenuation, Ray& scattered, Scene* scene
    ) const = 0;

    virtual bool terminate_ray(int depth, int min_depth, int max_depth, glm::dvec3& attenuation) const { return true;  }

};

class Unlit : public Material {
public:
    Unlit(const glm::dvec3& c) : color(c) {}

    virtual bool scatter(
        const Ray& r_in, const hit_record& rec, glm::dvec3& attenuation, Ray& scattered, Scene* scene
    ) const override;

    virtual bool terminate_ray(int depth, int min_depth, int max_depth, glm::dvec3& attenuation) {
        return true;
    }


public:
    glm::dvec3 color;
    shared_ptr<Texture> texture;
};

class Metal : public Material {
public:
    Metal(const glm::dvec3& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

    virtual bool scatter(
        const Ray& r_in, const hit_record& rec, glm::dvec3& attenuation, Ray& scattered, Scene* scene
    ) const override;

    virtual bool terminate_ray(int depth, int min_depth, int max_depth, glm::dvec3& attenuation) const override;

public:
    glm::dvec3 albedo;
    double fuzz;
};

class Lambertian :  public Material {
public:
    Lambertian(const glm::dvec3& a) : albedo(a) {}

    virtual bool scatter(
        const Ray& ray_in, const hit_record& rec, glm::dvec3& attentuation, Ray& scattered, Scene* scene
    ) const override;

    virtual bool terminate_ray(int depth, int min_depth, int max_depth, glm::dvec3& attenuation) const override;


public:
    glm::dvec3 albedo;
    std::shared_ptr<Texture> texture;
};

class Dielectric : public Material {
public:
    Dielectric(double index_of_refraction) : refraction_index(index_of_refraction) {}

    virtual bool scatter(
        const Ray& ray_in, const hit_record& rec, glm::dvec3& attentuation, Ray& scattered, Scene* scene
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