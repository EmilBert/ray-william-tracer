#pragma once

#include<iostream>

// Some shapes
#include"sphere.h"
#include"triangle.h"

// Others
#include"utility.h"
#include"color.h"
#include"hittable_list.h"
#include"camera.h"
#include"material.h"
#include "quad.h"

// Depth is the number of times our ray has bounced
glm::dvec3 ray_color(const Ray& ray, const Hittable& world, int depth) {
	
	hit_record rec;
	// We have exceeded the maximum number of bounce limits, no more light is generated!
	if(depth <= 0){
		return glm::dvec3(0,0,0);
	}

	// 0.001 is a thing we do to avoid shadow acne!
	if (world.hit(ray, 0.001, infinity, rec)) { // Check for hit and record some data
		Ray scattered;
		glm::dvec3 attenuation;
		if (rec.mat_ptr->scatter(ray, rec, attenuation, scattered)) {
			return attenuation * ray_color(scattered, world, depth-1);
			//return attenuation;
		}
		return glm::dvec3(0,0,0);
	}
	
	glm::dvec3 unit_direction = glm::normalize(ray.direction());
	double t = 0.5 * (unit_direction.y + 1.0);
	return (1.0 - t) * glm::dvec3(1.0, 1.0, 1.0) + t * glm::dvec3(0.5, 0.7, 1.0);

}

int main() {
	// Creating our camera
	Camera cam;

	// Some screen constants
    const auto aspect_ratio = 16.0 / 9.0;
    const int image_width = 400;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 100;
	const int max_depth = 50;

	// Creating and setting up our world
	HittableList world;
	auto material_ground = make_shared<Lambertian>(glm::dvec3(0.8, 0.8, 0.0));
    auto lambertian = make_shared<Lambertian>(glm::dvec3(0.1, 0.2, 0.4));
    //auto material_left   = make_shared<Metal>(Color(0.8, 0.8, 0.8), 0.3);
	//auto material_center = make_shared<Dielectric>(1.5);
	auto dielectric   = make_shared<Dielectric>(1.5);
    auto metal  = make_shared<Metal>(glm::dvec3(0.8, 0.6, 0.2), 1.0);
	auto unlit = make_shared<Unlit>(glm::dvec3(1.0, 0.0, 0.0));

    world.add(make_shared<Sphere>(glm::dvec3( 0.0, -100.5, -1.0), 100.0, material_ground));
    //world.add(make_shared<Sphere>(glm::dvec3( 0.0,    0.0, -1.0),   -0.4, dielectric));
    world.add(make_shared<Sphere>(glm::dvec3(-1.0,    0.0, -1.0),   0.5, lambertian)); // Negative radiance on dielectric material spheres gives a "hollow glass ball" effect, because of the direction the normals point
    world.add(make_shared<Sphere>(glm::dvec3( 1.0,    0.0, -1.0),   0.5, metal));

	TriangleData someData = {
		glm::dvec3(0,0,-2), glm::dvec3(2,0.5,-2), glm::dvec3(-2, 2, -2)
	};

	//world.add(make_shared<Triangle>(someData, glm::dvec3(0,0,0), 0, lambertian));
	//world.add(make_shared<Quad>(glm::dvec3(0, 0, -2), glm::dvec3(0, 2, -2), glm::dvec3(2, 0, -2), glm::dvec3(2, 2, -2), lambertian));

	std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

	for (int j = image_height - 1; j >= 0; --j) {
		std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
		for (int i = 0; i < image_width; ++i) {

			glm::dvec3 pixel_color(0,0,0);
			for(int s = 0; s < samples_per_pixel; ++s){
				// Super-sampling for better aliasing, use random
				double u = (i + random_double()) / (image_width-1);
				double v = (j + random_double()) / (image_height-1);
				Ray r = cam.get_ray(u,v);
				pixel_color += ray_color(r, world, max_depth);
			}

			// Write the final, super-sampled color
			write_color(std::cout, pixel_color, samples_per_pixel);
		}
	}

	std::cerr << "\nDone.\n";


}