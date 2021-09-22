#pragma once

#include "utility.h"
#include "hittable.h"
#include "Vec3.h"

class Material {
public:
    virtual bool scatter(
        const Ray& ray_in, const hit_record& rec, Color& attenuation, Ray& scattered
    ) const = 0;
};

class Unlit : public Material {
public:
    Unlit(const Color& c) : color(c) {}

    virtual bool scatter(
        const Ray& r_in, const hit_record& rec, Color& attenuation, Ray& scattered
    ) const override {
        attenuation = color;
        return true;
    }

public:
    Color color;
};

class Metal : public Material {
public:
    Metal(const Color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

    virtual bool scatter(
        const Ray& r_in, const hit_record& rec, Color& attenuation, Ray& scattered
        ) const override {
        Vec3 refelected = reflect(unit_vector(r_in.direction()), rec.normal);
        scattered = Ray(rec.p, refelected + fuzz*random_in_unit_sphere());
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }

public:
    Color albedo;
    double fuzz;
};

class Lambertian :  public Material {
public:
    Lambertian(const Color& a) : albedo(a) {}

    virtual bool scatter(
        const Ray& ray_in, const hit_record& rec, Color& attentuation, Ray& scattered
    ) const override {
        auto scatter_direction = rec.normal + random_unit_vector();
        
        //Catch degenrate scatter direction
        if(scatter_direction.near_zero())
            scatter_direction = rec.normal;
        
        scattered = Ray(rec.p, scatter_direction);
        attentuation = albedo;
        return true;
    }

public:
    Color albedo;
};

class Dielectric : public Material {
public:
    Dielectric(double index_of_refraction) : refraction_index(index_of_refraction) {}

    virtual bool scatter(
        const Ray& ray_in, const hit_record& rec, Color& attentuation, Ray& scattered
    ) const override {
        attentuation = Color(1.0, 1.0, 1.0);
        double refraction_ratio = rec.front_face ? (1.0 / refraction_index) : refraction_index;
        Vec3 unit_direction = unit_vector(ray_in.direction());
        Vec3 refracted = refract(unit_direction, rec.normal, refraction_ratio);

        double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = sqrt(1.0 - cos_theta * cos_theta); // Trig-ettan 
        bool cannot_refract = refraction_ratio * sin_theta > 1.0;
        Vec3 direction;

        // This is some weird hack to simulate how glass looks at an angle
        if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double())
            direction = reflect(unit_direction, rec.normal);
        else
            direction = refract(unit_direction, rec.normal, refraction_ratio);

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