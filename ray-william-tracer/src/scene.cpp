#include"scene.h"
#include<iostream>
#include<fstream>
#include"hittable.h"
#include"material.h"
#include"triangle.h"
#include"sphere.h"
#include"color.h"
#include"light.h"
#include"sdl_rendering.h"

#include<glm/vec3.hpp>
#include<atomic>
#include<thread>
#include<vector>
#include<future>
#include<asyncinfo.h>

#define RAY_WILLIAM_MULTI_THREAD true

Scene::Scene() : camera(), world()
{
	framebuffer = new glm::dvec3[camera.image_width * camera.image_height];
}

void Scene::setup_scene()
{
	auto material_ground = make_shared<Lambertian>(glm::dvec3(1.0, 1.0, 1.0));
	auto lambertian_red = make_shared<Lambertian>(glm::dvec3(1.0, 0, 0));
	auto lambertian_green = make_shared<Lambertian>(glm::dvec3(0, 1.0, 0));

	const auto intensity = 4.0;
	auto diffuse_light = make_shared<Diffuse_light>(glm::dvec3(intensity)); //Can be put to higher values if need be

	auto dielectric = make_shared<Dielectric>(1.5);
	auto metal = make_shared<Metal>(glm::dvec3(1.0, 1.0, 1.0), 0);
	auto unlit = make_shared<Unlit>(glm::dvec3(1.0, 0.0, 0.0));

	add_room(glm::dvec3(0, 0, -1), 1, material_ground	, lambertian_red, lambertian_green);
	//addCube(glm::dvec3(0.5, 0.5, -1.5), 0.2, dielectric, world, glm::dvec3(0, 20.0, 0));
	//addCube(glm::dvec3(0, 0, -1), 0.1, diffuse_light, world, glm::dvec3(0, 0, 0));
	world.add(make_shared<Sphere>(glm::dvec3(0.2, 0.0, -1.2), 0.3, lambertian_green));
	world.add(make_shared<Sphere>(glm::dvec3(-0.35, 0.0, -1.2), 0.2, lambertian_red));
	double eps = 1e-06;
	double y = 1 - eps;
	double z = -1;
	double size = 0.40;
	double x = 0;
	//add_quad(glm::dvec3(x + size, y, z - size), glm::dvec3(x - size, y, z - size), glm::dvec3(x + size, y, z + size), glm::dvec3(x - size, y, z + size), unlit);

	std::vector<glm::dvec3> v = { glm::dvec3(x + size, y, z + size), glm::dvec3(x - size, y, z + size), glm::dvec3(x + size, y, z - size), glm::dvec3(x - size, y, z - size) };
	world.add(make_shared<Light>(v, glm::dvec3(x, y, z)));

	//world.add(make_shared<Triangle>(someData, glm::dvec3(0,0,0), 0, lambertian));
	//world.add(make_shared<Quad>(glm::dvec3(0, 0, -2), glm::dvec3(0, 2, -2), glm::dvec3(2, 0, -2), glm::dvec3(2, 2, -2), lambertian));

}

void Scene::render_scene()
{
	glm::dvec3 bg = { 1.0, 1.0, 1.0 };

#if RAY_WILLIAM_MULTI_THREAD 
	// Multi-threading additions
	int max = camera.image_width * camera.image_height;
	std::size_t cores = std::thread::hardware_concurrency();
	volatile std::atomic<int> count(0);
	std::vector<std::future<void>> future_vector;

	int width = camera.image_width;
	int height = camera.image_height;

	// Loop for each core
	std::cout << "Starting the render with multi-core, hold on to your hat" << std::endl;
	while (cores--) {
		Scene* scene = this;
		future_vector.emplace_back(
			std::async([=, &count, &scene]() { // = sign to capture all local variables as lvalues
				while (true) {
					// Increase the count and check for termination
					int index = count++;
					if (index >= max)
						break;

					// Grab our x and y positon
					int x = index % width;
					int y = index / width;

					glm::dvec3 pixel_color(0, 0, 0);
					for (int s = 0; s < camera.samples_per_pixel; ++s) {
						// Do our uv
						double u = (x + random_double()) / (width - 1);
						double v = 1 - ((y + random_double()) / (height - 1));

						// Shoot ray!
						Ray r = camera.get_ray(u, v);
						pixel_color += ray_color(r, bg, scene->world, camera.max_depth);
					}

					// Set pixel_color at index to pixel_color
					framebuffer[index] = pixel_color;
				}
		}));
	}
#else
	glm::dvec3* pixel = framebuffer; // Current pixel we write to

	for (int j = camera.image_height - 1; j >= 0; --j) {
		std::cout << "\rScanlines remaining: " << j << ' ' << std::flush;
		// Progress logging?

		for (int i = 0; i < camera.image_width; ++i) {

			glm::dvec3 pixel_color(0, 0, 0);
			for (int s = 0; s < camera.samples_per_pixel; ++s) {
				// Super-sampling for better aliasing, use random
				double u = (i + random_double()) / (camera.image_width - 1);
				double v = (j + random_double()) / (camera.image_height - 1);
				Ray r = camera.get_ray(u, v);
				pixel_color += ray_color(r, bg, world, camera.max_depth);
			}
			//pixel_color = ray_color(camera.get_ray(i,j), bg, world, camera.max_depth);

			// Write the final, super-sampled color to our framebuffer
			// Dereference the memory adress and stride forward in memory
			*(pixel++) = pixel_color;
		}
	}
#endif
}


void Scene::pixel_pass()
{
	// Pass for now
	for (int i = 0; i < camera.image_width * camera.image_height; i++) {
		glm::dvec3 color = framebuffer[i];

		color = { 1 - color.r, 1 - color.g, 1 - color.b };

		framebuffer[i] = color;
	}
}

void Scene::write_render_to_file(const std::string& image_name)
{
	// Create new ppm filestream
	std::ofstream output_stream(image_name);

	// Write image buffer to file
	output_stream << "P3\n" << camera.image_width << ' ' << camera.image_height << "\n255\n";
	for (uint32_t i = 0; i < camera.image_width * camera.image_height; ++i) {
		write_color(output_stream, framebuffer[i], camera.samples_per_pixel);
	}

	// Clean up
	output_stream.close();
	delete[] framebuffer;
	framebuffer = nullptr;
}

void Scene::view_render_in_SDL() const
{
	// So basically, open a SDL window, take our framebuffer and draw each pixel, shouldn't be too hard - Daivd on 13th of October 2021
	// famous last words
	render_framebuffer_as_SDL(framebuffer, camera.image_width, camera.image_height);
}

glm::dvec3 Scene::ray_color(const Ray& ray, glm::dvec3 bg, const Hittable& world, int depth) const
{
	hit_record rec;
	// We have exceeded the maximum number of bounce limits, no more light is generated!
	if(depth <= 0){
		return glm::dvec3(0,0,0);
	}

	// 0.001 is a thing we do to avoid shadow acne!
	if (world.hit(ray, 0.001, infinity, rec)) { // Check for hit and record some data
		Ray scattered;
		glm::dvec3 attenuation;
		// glm::dvec3 emitted = rec.mat_ptr->emitted(rec.p);

		if ((*rec.hittable_ptr).isLight()) {
			return glm::dvec3(1, 1, 1);
		}

		rec.mat_ptr->scatter(ray, rec, attenuation, scattered);

		// Check if hit point is illuminated or shadowed?
		bool hitSomething = false;
		for (int i : this->world.light_indices) {
			// Get some data 
			auto l = dynamic_cast<Light*>(this->world.objects[i].get());

			glm::dvec3 position = l->position;
			glm::dvec3 toLight = glm::normalize(position - rec.p);

			// Shoot our ray, TOOD: do it for each sample point for light
			Ray pointToLight(rec.p, toLight);
			hit_record light_record; // unused

			// Shoot ray and see if we hit something
			if (world.hit(pointToLight, 0.001, infinity, light_record)) {
				if (glm::distance(rec.p, light_record.p) < glm::distance(rec.p, position)) {
					// Object in front of us
					hitSomething = true;
				}
			}
		}

		return (hitSomething ? 0.5 : 1.0) * attenuation * ray_color(scattered, bg, world, depth - 1);
		//return (double)!hitSomething * attenuation * ray_color(scattered, bg, world, depth - 1);
	}

	return bg; 
}

void Scene::add_quad(const glm::dvec3& bottomLeft, const glm::dvec3& bottomRight, const glm::dvec3& topLeft, const glm::dvec3& topRight, shared_ptr<Material> m)
{
    TriangleData someData = {
		bottomLeft, bottomRight, topLeft
	};
	TriangleData someData2 = {
		bottomRight, topRight, topLeft
	};
	glm::dvec3 rotation = { 0,0,0 };
	world.add(make_shared<Triangle>(someData, rotation, m));
	world.add(make_shared<Triangle>(someData2, rotation, m));
}

void Scene::add_room(const glm::dvec3& origin, double radius, shared_ptr<Material> m, shared_ptr<Material> left, shared_ptr<Material> right)
{
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

	glm::dvec3 translation = { 0,0,0.2 };

	add_quad(v4, v0, v5, v1, left); // left
	add_quad(v0+translation, v3+translation, v1+translation, v2+translation, m); // front wall
	add_quad(v3, v7, v2, v6, right);  // höger väggen
	//add_quad(v7,v4,v6,v5, m); // <- backwall
	add_quad(v4, v7, v0, v3, m); // golvet
	add_quad(v1, v2, v5, v6, m); // taket
}

void Scene::add_cube(const glm::dvec3& origin, double radius, shared_ptr<Material> m, HittableList& world_ref, glm::dvec3 rot)
{
    glm::dvec3 o(origin);
	glm::dquat rotQuad;
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
	
    v0 = o + rotQuad * (v0 - o);
	v1 = o + rotQuad * (v1 - o);
	v2 = o + rotQuad * (v2 - o);
	v3 = o + rotQuad * (v3 - o);
	v4 = o + rotQuad * (v4 - o);
	v5 = o + rotQuad * (v5 - o);
	v6 = o + rotQuad * (v6 - o);
	v7 = o + rotQuad * (v7 - o);

	add_quad(v0, v4, v1, v5, m);
	add_quad(v3, v0, v2, v1, m);
	add_quad(v7, v3, v6, v2, m);
	add_quad(v4, v7, v5, v6, m);
	add_quad(v7, v4, v3, v0, m);
	add_quad(v2, v1, v6, v5, m);
}



