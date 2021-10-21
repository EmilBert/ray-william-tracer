#include"material.h"

#include"light.h"
#include"scene.h"

#include<glm/common.hpp>

/* METAL */

bool Metal::scatter(const Ray& r_in, const hit_record& rec, glm::dvec3& attenuation, Ray& scattered, Scene* scene) const
{
    glm::dvec3 reflected = glm::reflect(glm::normalize(r_in.direction()), rec.normal);
    scattered = Ray(rec.p, reflected);
    attenuation = 0.8*albedo;
    return (glm::dot(scattered.direction(), rec.normal) > 0);
}

bool Metal::terminate_ray(int depth, int min_depth, int max_depth, glm::dvec3& attenuation) const
{
    return depth > max_depth;
}

/* LAMBERTIAN */

bool Lambertian::scatter(const Ray& ray_in, const hit_record& rec, glm::dvec3& attentuation, Ray& scattered, Scene* scene) const
{
    // TODO: Russian roulette implementation (see lecture 10 notes)
    auto scatter_direction = rec.normal + random_unit_vector();

    //Catch degenrate scatter direction
    if (isVectorNearZero(scatter_direction))
        scatter_direction = rec.normal;

    scattered = Ray(rec.p, scatter_direction);

    // Get light diffusion
    double diffusion = glm::max(0.3, scene->light_ray_pass(rec));

    attentuation = diffusion * albedo;
    return true;
}

bool Lambertian::terminate_ray(int depth, int min_depth, int max_depth, glm::dvec3& attenuation) const
{
    if (++depth > min_depth) {
        // Threshold creation, grab the maximum color value
        double p = glm::max(attenuation.r, glm::max(attenuation.g, attenuation.b));
        if (random_double() > p) {
            // Break
            return true;
        }
        else {
            attenuation *= 1 / p;
        }
    }

    return false;
}

/* DIELECTRIC */

bool Dielectric::scatter(const Ray& ray_in, const hit_record& rec, glm::dvec3& attentuation, Ray& scattered, Scene* scene) const
{
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
