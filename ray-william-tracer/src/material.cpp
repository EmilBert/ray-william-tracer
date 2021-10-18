#include"material.h"

#include"light.h"

/* METAL */

bool Metal::scatter(const Ray& r_in, const hit_record& rec, glm::dvec3& attenuation, Ray& scattered) const
{
    glm::dvec3 reflected = glm::reflect(glm::normalize(r_in.direction()), rec.normal);
    scattered = Ray(rec.p, reflected + fuzz * random_in_unit_sphere());
    attenuation = albedo;
    return (glm::dot(scattered.direction(), rec.normal) > 0);
}

glm::dvec3 Metal::light_pass(const glm::dvec3& light_pos, const hit_record& rec, Light* l)
{
    return glm::dvec3(0, 0, 0);
}

/* LAMBERTIAN */

bool Lambertian::scatter(const Ray& ray_in, const hit_record& rec, glm::dvec3& attentuation, Ray& scattered) const
{
    // TODO: Russian roulette implementation (see lecture 10 notes)
    auto scatter_direction = rec.normal + random_unit_vector();

    //Catch degenrate scatter direction
    if (isVectorNearZero(scatter_direction))
        scatter_direction = rec.normal;

    scattered = Ray(rec.p, scatter_direction);
    attentuation = albedo;
    return true;
}

glm::dvec3 Lambertian::light_pass(const glm::dvec3& light_pos, const hit_record& rec, Light* l)
{
    glm::dvec3 toLight = (light_pos - rec.p); // vector pointing to our randomLightPos
    glm::dvec3 toLightNormalized = glm::normalize(toLight);

    double cosThetaIn = glm::dot(toLightNormalized, rec.normal);
    double cosThetaL = glm::dot(-toLightNormalized, l->t0.normal);

    glm::dvec3 result = l->light_color * ((cosThetaIn * cosThetaL) / glm::length(toLight)) * l->intensity;

    return result;
}

/* DIELECTRIC */

bool Dielectric::scatter(const Ray& ray_in, const hit_record& rec, glm::dvec3& attentuation, Ray& scattered) const
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
