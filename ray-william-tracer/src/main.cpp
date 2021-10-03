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

// Depth is the number of times our ray has bounced
glm::dvec3 ray_color(const Ray& ray, const Hittable& world, int depth) {
	
	hit_record rec;
	// We have exceeded the maximum number of bounce limits, no more light is generated!
	if(depth <= 0){
		return glm::dvec3(0,0,0);
	}

	// 0.001 is a thing we do to avoid shadow acne!
	if (world.hit(ray, 0.01, infinity, rec)) { // Check for hit and record some data
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

void addQuad(const glm::dvec3& position, const glm::dvec3& rotation, shared_ptr<Material> m, HittableList& world_ref) {

	glm::dvec3 bottomLeft = { -2, 0, 0 };
	glm::dvec3 bottomRight = { 2,0, 0 };
	glm::dvec3 topLeft = { -2, 4, 0 };
	glm::dvec3 topRight = { 2,4, 0 };

	TriangleData someData = {
		position + bottomLeft, position + bottomRight, position + topLeft
	};
	TriangleData someData2 = {
		position + bottomRight, position + topRight, position + topLeft
	};

	world_ref.add(make_shared<Triangle>(someData, rotation, m));
	world_ref.add(make_shared<Triangle>(someData2, rotation, m));
}

void addQuad(const glm::dvec3& bottomLeft, const glm::dvec3& bottomRight, const glm::dvec3& topLeft, const glm::dvec3& topRight, shared_ptr<Material> m, HittableList& world_ref) {
	TriangleData someData = {
		bottomLeft, bottomRight, topLeft
	};
	TriangleData someData2 = {
		bottomRight, topRight, topLeft
	};

	glm::dvec3 rotation = { 0,0,0 };

	world_ref.add(make_shared<Triangle>(someData, rotation, m));
	world_ref.add(make_shared<Triangle>(someData2, rotation, m));
}

// Inward facing normals
void addRoom(const glm::dvec3& origin, double radius, shared_ptr<Material> m, HittableList& world_ref) {

	double x1 = origin.x - radius;
	double x2 = origin.x + radius;
	double y1 = origin.y - radius;
	double y2 = origin.y + radius;
	double z1 = origin.z - radius;
	double z2 = origin.z + radius;

	glm::dvec3 v0{ x1, y1, z1 };
	glm::dvec3 v1{ x1, y2, z1 };
	glm::dvec3 v2{ x2, y2, z1 };
	glm::dvec3 v3{ x2, y1, z1 };
	glm::dvec3 v4{ x1, y1, z2 };
	glm::dvec3 v5{ x1, y2, z2 };
	glm::dvec3 v6{ x2, y2, z2 };
	glm::dvec3 v7{ x2, y1, z2 };
	
	addQuad(v4,v0,v5,v1, m, world_ref);
	addQuad(v0,v3,v1,v2, m, world_ref);
	addQuad(v3,v7,v2,v6, m, world_ref);
	//addQuad(v7,v4,v6,v5, m, world_ref); // <- backwall
	addQuad(v4,v7,v0,v3, m, world_ref);
	addQuad(v1,v2,v5,v6, m, world_ref);
}

// Outward facing normals
void addCube(const glm::dvec3& origin, double radius, shared_ptr<Material> m, HittableList& world_ref, glm::dvec3 rot) {


	glm::vec3 o(origin);
	glm::quat rotQuad;
	rotQuad = glm::quat(glm::radians(rot));

	double x1 = origin.x - radius;
	double x2 = origin.x + radius;
	double y1 = origin.y - radius;
	double y2 = origin.y + radius;
	double z1 = origin.z - radius;
	double z2 = origin.z + radius;

	glm::dvec3 v0{ x1, y1, z1 };
	glm::dvec3 v1{ x1, y2, z1 };
	glm::dvec3 v2{ x2, y2, z1 };
	glm::dvec3 v3{ x2, y1, z1 };
	glm::dvec3 v4{ x1, y1, z2 };
	glm::dvec3 v5{ x1, y2, z2 };
	glm::dvec3 v6{ x2, y2, z2 };
	glm::dvec3 v7{ x2, y1, z2 };

	// Rotation on local axis
	//v0 = o + rotQuad * (glm::vec3(v0) - o);

	// Global Rotation
	//v0 = rotQuad * glm::vec3(v0);

	v0 = o + rotQuad * (glm::vec3(v0) - o);
	v1 = o + rotQuad * (glm::vec3(v1) - o);
	v2 = o + rotQuad * (glm::vec3(v2) - o);
	v3 = o + rotQuad * (glm::vec3(v3) - o);
	v4 = o + rotQuad * (glm::vec3(v4) - o);
	v5 = o + rotQuad * (glm::vec3(v5) - o);
	v6 = o + rotQuad * (glm::vec3(v6) - o);
	v7 = o + rotQuad * (glm::vec3(v7) - o);

	addQuad(v0, v4, v1, v5, m, world_ref);
	addQuad(v3, v0, v2, v1, m, world_ref);
	addQuad(v7, v3, v6, v2, m, world_ref);
	addQuad(v4, v7, v5, v6, m, world_ref);
	addQuad(v7, v4, v3, v0, m, world_ref);
	addQuad(v2, v1, v6, v5, m, world_ref);
}



int main() {
	// Creating our camera
	Camera cam;

	// Some screen constants
    const auto aspect_ratio = 16.0 / 9.0;
    const int image_width = 650;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 60;
	const int max_depth = 50;

	// Creating and setting up our world
	HittableList world;
	auto material_ground = make_shared<Lambertian>(glm::dvec3(0.8, 0.8, 0.0));
    auto lambertian = make_shared<Lambertian>(glm::dvec3(0.1, 0.2, 0.4));
    //auto material_left   = make_shared<Metal>(Color(0.8, 0.8, 0.8), 0.3);
	//auto material_center = make_shared<Dielectric>(1.5);
	auto dielectric   = make_shared<Dielectric>(1.5);
    auto metal  = make_shared<Metal>(glm::dvec3(0.8, 0.6, 0.2), 0.2);
	auto unlit = make_shared<Unlit>(glm::dvec3(1.0, 0.0, 0.0));

    //world.add(make_shared<Sphere>(glm::dvec3( 0.0, -100.5, -1.0), 100.0, material_ground));
    //world.add(make_shared<Sphere>(glm::dvec3( 0.0,    0.0, -1.0),   -0.4, dielectric));
    //world.add(make_shared<Sphere>(glm::dvec3(-1.0,    0.0, -1.0),   0.5, lambertian)); // Negative radiance on dielectric material spheres gives a "hollow glass ball" effect, because of the direction the normals point
    //world.add(make_shared<Sphere>(glm::dvec3( 1.0,    0.0, -1.0),   0.5, metal));

	//world.add(make_shared<Triangle>(someData, glm::dvec3(0,0,0), 0, lambertian));
	//world.add(make_shared<Quad>(glm::dvec3(0, 0, -2), glm::dvec3(0, 2, -2), glm::dvec3(2, 0, -2), glm::dvec3(2, 2, -2), lambertian));

	glm::dvec3 position = { -1, -2, -4 };
	glm::dvec3 rotation = { 0, 0, 0 };
	//addQuad(position, rotation, lambertian, world);

	// Left wall
	glm::dvec3 bottomLeft = { -2, -1, -1 };
	glm::dvec3 bottomRight = { -1, -1, -3 };
	glm::dvec3 topLeft = { -2, 1, -1 };
	glm::dvec3 topRight = { -1, 1, -3 };
	//addQuad(bottomLeft, bottomRight, topLeft, topRight, lambertian, world);

	// Right wall
	bottomLeft = { -2, -1, -3 };
	bottomRight = { -1, -1, -3 };
	topLeft = { -2, 1, -1 };
	topRight = { -1, 1, -3 };
	addRoom(glm::dvec3(0, 0, -1), 1, lambertian, world);
	addCube(glm::dvec3(0.5, 0.5, -1.5), 0.2, dielectric, world, glm::dvec3(0, 20.0, 0));
	addCube(glm::dvec3(-0.5, -0.5, -1.5), 0.2, metal, world, glm::dvec3(0,45.0,0));

	

	//addQuad(bottomLeft, bottomRight, topLeft, topRight, lambertian, world);

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