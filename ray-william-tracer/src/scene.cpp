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
#include"plane.h"
#include"quad.h"
#include"texture.h"

#define NOMINMAX
#include<glm/vec3.hpp>
#include<glm/common.hpp>
#include<atomic>
#include<thread>
#include<vector>
#include<future>
#include <chrono>

#define RAY_WILLIAM_MULTI_THREAD true
#define RECORD_RENDER_TIME false
#define MIN_LIGHT_INTENSITY 0

Scene::Scene() : camera(glm::dvec3(0,0,-0.1), glm::dvec3(0,0,-1), glm::dvec3(0,1,0), 90, 12.0/9.0), world()
{
}

void Scene::setup_scene()
{
//#define SPHERE_UNLIT_SCENE
//#define SPHERE_DIFFUSE_SCENE1
//#define SCENEFOR252
//#define CORNELL_BOX_WITH_LIGHTS_AND_SHADOWS
#define DAVE_SCENE

#ifdef SPHERE_UNLIT_SCENE
	auto unlit_blue = make_shared<Unlit>(glm::dvec3(0, 0, 1));
	world.add(make_shared<Sphere>(glm::dvec3(0, 0, -1), 0.5, unlit_blue));
#endif

#ifdef SPHERE_DIFFUSE_SCENE1
	auto lambertian_blue = make_shared<Lambertian>(glm::dvec3(0.1, 0.1, 1.0));
	auto lambertian_green = make_shared<Lambertian>(glm::dvec3(0.1, 1.0, 0.1));
	world.add(make_shared<Sphere>(glm::dvec3(0, 0, -1), 0.5, lambertian_blue));
	world.add(make_shared<Sphere>(glm::dvec3(0, -100.5, -1), 100, lambertian_green));
#endif

#ifdef SCENEFOR252
	auto lambertian_blue = make_shared<Lambertian>(glm::dvec3(0.1, 0.1, 1.0));
	auto lambertian_green = make_shared<Lambertian>(glm::dvec3(0.1, 1.0, 0.1));
	auto mirror = make_shared<Metal>(glm::dvec3(1, 1, 1), 0.0);

	world.add(make_shared<Sphere>(glm::dvec3(-0.6, 0, -1), 0.5, lambertian_blue));
	world.add(make_shared<Sphere>(glm::dvec3(0.6, 0, -1), 0.5, mirror));
	world.add(make_shared<Sphere>(glm::dvec3(0, -100.5, -1), 100, lambertian_green));
#endif

#ifdef CORNELL_BOX_WITH_LIGHTS_AND_SHADOWS
	auto right_wall = color_255_to_01({ 61, 122, 179 });
	auto left_wall = color_255_to_01({ 200, 93, 211 });
	auto right = make_shared<Lambertian>(right_wall);
	auto left = make_shared<Lambertian>(left_wall);
	auto m = make_shared<Lambertian>(glm::dvec3(1.0, 1.0, 1.0));
	add_cornell_box(glm::dvec3(0, 0, -1), 1, m, left, right);

	double eps = 1e-06;
	double y = 1 - eps;
	double z = -1;
	double size = 0.35;
	double x = 0;
	std::vector<glm::dvec3> v = { glm::dvec3(x + size, y, z + size), glm::dvec3(x - size, y, z + size), glm::dvec3(x + size, y, z - size), glm::dvec3(x - size, y, z - size) };
	world.add(make_shared<Light>(v, glm::dvec3(x, y, z), 5.0, glm::dvec3{ 1, 1, 1 }));

	auto lambertian = make_shared<Lambertian>(color_255_to_01(75, 0, 130));
	auto dielectric = make_shared<Dielectric>(1.3);
	world.add(make_shared<Sphere>(glm::dvec3(0, 0, -1.2), 0.25, dielectric));

#endif

#ifdef DAVE_SCENE
	auto right_wall = color_255_to_01({ 61, 122, 179 });
	auto left_wall = color_255_to_01({ 200, 93, 211 });
	auto right = make_shared<Lambertian>(right_wall);
	auto left = make_shared<Lambertian>(left_wall);
	auto m = make_shared<Lambertian>(glm::dvec3(1.0, 1.0, 1.0));
	//add_cornell_box(glm::dvec3(0, 0, -1), 1, m, left, right);
	
	double eps = 1e-06;
	double y = 2 - eps;
	double z = -1;
	double size = 1.5;
	double x = 0;
	std::vector<glm::dvec3> v = { glm::dvec3(x + size, y, z + size), glm::dvec3(x - size, y, z + size), glm::dvec3(x + size, y, z - size), glm::dvec3(x - size, y, z - size) };
	//world.add(make_shared<Light>(v, glm::dvec3(x, y, z), 3.0, glm::dvec3{ 1, 1, 1 }));

	// Add skybox
	add_cube_map("images/top.jpg", "images/bottom.jpg", "images/left.jpg", "images/right.jpg", "images/front.jpg", "images/back.jpg");

	// Add sphere
	auto dielectric = make_shared<Dielectric>(2);

	auto texture = make_shared<ImageTexture>("images/brick_base.jpg");
	auto normal = make_shared<ImageTexture>("images/brick_normal.jpg");
	auto brick_texture = make_shared<Lambertian>(glm::dvec3(1, 1, 1));
	brick_texture->texture = texture;
	brick_texture->normal_map = normal;
	world.add(make_shared<Sphere>(glm::dvec3(0, 0, -1), 0.3, dielectric));

#endif

}

void Scene::render_scene()
{
	if (framebuffer != nullptr) delete[] framebuffer;
	framebuffer = new glm::dvec3[camera.image_width * camera.image_height];

	/*
	Go through each light, trace rays, store in data-structure
	2 maps:
	global photon map
	caustics photon map
	*/
	
#if RECORD_RENDER_TIME
	auto start_time = std::chrono::high_resolution_clock::now();
#endif

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
						pixel_color += trace_ray(r, scene->world, 0);
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
				pixel_color += trace_ray(r, world, 0);
			}
			//pixel_color = ray_color(camera.get_ray(i,j), bg, world, camera.max_depth);

			// Write the final, super-sampled color to our framebuffer
			// Dereference the memory adress and stride forward in memory
			*(pixel++) = pixel_color;
		}
	}
#endif

#if RECORD_RENDER_TIME
	auto stop_time = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::seconds>(stop_time - start_time);
	std::cout << "Render time in seconds: " << duration.count() << std::endl;
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

void Scene::scene1()
{
	Scene::camera = Camera(glm::dvec3(0.2, 0.6, -0.3), glm::dvec3(0, -0.2, -1), glm::dvec3(0, 1, 0), 90, 16.0 / 9.0);

	world.clear();
	add_final_scene();
	
	auto lambertian_object = make_shared<Lambertian>(color_255_to_01(44, 94, 46));
	auto glass = make_shared<Dielectric>(1.5);
	auto mirror = make_shared<Metal>(glm::dvec3(1.0, 1.0, 1.0), 0);
	auto fuzzy_metal = make_shared<Metal>(glm::dvec3(1.0, 1.0, 1.0), 0.2);

	add_cube(glm::dvec3(0.3, 0.3, -1.3), 0.25, fuzzy_metal, world, glm::dvec3(0, 20.0, 0));
	world.add(make_shared<Sphere>(glm::dvec3(-0.3, 0.2, -0.7), 0.20, mirror));
	world.add(make_shared<Sphere>(glm::dvec3(0.3, -0.2, -0.7), 0.25, glass));
	add_cube(glm::dvec3(-0.3, -0.4, -1.3), 0.2, lambertian_object, world, glm::dvec3(0, 30.0, 0));
}

void Scene::scene2()
{
	Scene::camera = Camera(glm::dvec3(0.2, 0.5, -0.3), glm::dvec3(0, 0.3, -1), glm::dvec3(0, 1, 0), 90, 16.0 / 9.0);

	world.clear();
	add_final_scene();

	auto lambertian_object = make_shared<Lambertian>(color_255_to_01(44, 94, 46));
	auto glass = make_shared<Dielectric>(1.5);
	auto mirror = make_shared<Metal>(glm::dvec3(1.0, 1.0, 1.0), 0);
	auto fuzzy_metal = make_shared<Metal>(glm::dvec3(1.0, 1.0, 1.0), 0.2);

	add_cube(glm::dvec3(0.3, -0.2, -0.7), 0.2, fuzzy_metal, world, glm::dvec3(20.0, 40.0, 0));
	world.add(make_shared<Sphere>(glm::dvec3(-0.3, 0.2, -0.7), 0.35, mirror));
	world.add(make_shared<Sphere>(glm::dvec3(0.3, 0.4, -1.3), 0.35, glass));
	add_cube(glm::dvec3(-0.3, -0.4, -1.3), 0.2, lambertian_object, world, glm::dvec3(0, 30.0, 0));
}

void Scene::scene3()
{
	Scene::camera = Camera(glm::dvec3(0.2, 0.5, -0.3), glm::dvec3(0, 0.3, -1), glm::dvec3(0, 1, 0), 90, 16.0 / 9.0);

	world.clear();
	add_final_scene();

	auto lambertian_object = make_shared<Lambertian>(color_255_to_01(44, 94, 46));
	auto glass = make_shared<Dielectric>(1.5);
	auto mirror = make_shared<Metal>(glm::dvec3(1.0, 1.0, 1.0), 0);
	auto fuzzy_metal = make_shared<Metal>(glm::dvec3(1.0, 1.0, 1.0), 0.2);

	add_cube(glm::dvec3(0.3, 0.4, -1.3), 0.2, fuzzy_metal, world, glm::dvec3(0, 20.0, 0));
	world.add(make_shared<Sphere>(glm::dvec3(-0.3, 0.2, -0.7), 0.35, mirror));
	world.add(make_shared<Sphere>(glm::dvec3(0.3, -0.2, -0.7), 0.35, glass));
	add_cube(glm::dvec3(-0.3, -0.4, -1.3), 0.2, lambertian_object, world, glm::dvec3(0, 30.0, 0));
}


void Scene::view_render_in_SDL() const
{
	// So basically, open a SDL window, take our framebuffer and draw each pixel, shouldn't be too hard - Daivd on 13th of October 2021
	// famous last words
	render_framebuffer_as_SDL(framebuffer, camera.image_width, camera.image_height, camera.samples_per_pixel);
}

glm::dvec3 Scene::trace_ray(const Ray& ray, const Hittable& world, int depth) const
{
	hit_record rec;

	// 0.001 is a thing we do to avoid shadow acne!
	if (world.hit(ray, 0.001, infinity, rec)) { // Check for hit and record some data
		Ray scattered;
		glm::dvec3 attenuation;

		// If we hit a light simply terminate and return tht lights color
		if ((*rec.hittable_ptr).isLight()) {
			return dynamic_cast<Light*>(rec.hittable_ptr.get())->light_color;
		}

		// Mateiral scattering, takes light into account
		// Light contribution gets spent into attenuation
		rec.mat_ptr->scatter(ray, rec, attenuation, scattered, (Scene*)this);

		// Do we terminate?
		if (rec.mat_ptr->terminate_ray(depth, min_depth, max_depth, attenuation)) {
			return attenuation;
		}

		// Send another ray with recursion in the scattered direction
		return attenuation * trace_ray(scattered, world, ++depth);
	}

	//return bg; 
	glm::dvec3 unit_direction = glm::normalize(ray.direction());
	auto t = 0.5 * (unit_direction.y + 1.0);
	return (1.0 - t) * glm::dvec3(1.0, 1.0, 1.0) + t * glm::dvec3(0.5, 0.7, 1.0);
}

double Scene::light_ray_pass(const glm::dvec3& position, const glm::dvec3& normal) const
{
	double brightness = 0.0;
	for (auto l : world.lights) {

		const int N = 6;
		//glm::dvec3 G = { 0,0,0 };
		double G = 0.0;

		for (int i = 0; i < N; i++) {
			// For each sample point shoot a ray to a random point at the light
			glm::dvec3 randomLightPos = l->getRandomPosition();

			bool seenByLight = true; // flag
			glm::dvec3 toLight = (randomLightPos - position); // vector pointing to our randomLightPos
			glm::dvec3 toLightNormalized = glm::normalize(toLight);

			// Shoot our ray to sample point
			Ray pointToSamplePoint(position, toLightNormalized);
			hit_record light_record; // unused
			if (world.hit(pointToSamplePoint, 0.001, infinity, light_record)) {
				if (glm::distance(position, light_record.p) < glm::distance(position, randomLightPos)) {
					seenByLight = false;
				}
			}

			if (seenByLight) {
				// From mark's lecture, somehow works, what's the difference from normal diffuse shading?
				double cosThetaIn = glm::dot(toLightNormalized, normal);
				double cosThetaL = glm::dot(-toLightNormalized, l->t0.normal);
				G += (cosThetaIn * cosThetaL) / glm::length(toLight) * l->intensity;
			}
		}
		// We take a total of N samples pointing to the light
		brightness += G / (double)N;
	}

	return brightness;
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

void Scene::add_cornell_box(const glm::dvec3& origin, double radius, shared_ptr<Material> m, shared_ptr<Material> left, shared_ptr<Material> right)
{
	world.add(make_shared<Plane>(origin + glm::dvec3(0, -radius, 0), glm::dvec3(0, 1, 0), m)); // Floor
	world.add(make_shared<Plane>(origin + glm::dvec3(0, radius, 0), glm::dvec3(0, -1, 0), m)); // Roof
	world.add(make_shared<Plane>(origin + glm::dvec3(-radius, 0, 0), glm::dvec3(1, 0, 0), left)); // Left
	world.add(make_shared<Plane>(origin + glm::dvec3(radius, 0, 0), glm::dvec3(-1,0,0), right)); // Right
	world.add(make_shared<Plane>(origin + glm::dvec3(0, 0, radius), glm::dvec3(0, 0, -1), m)); // Front
	world.add(make_shared<Plane>(origin + glm::dvec3(0, 0, -radius), glm::dvec3(0, 0, 1), m)); // Back
}

/**
	Mark-room Description:

	xz-plane:

	*v6	  __v2__	 *v7
	  _--=	    =--_
	v1				v3
	|				 |
	|				 |
	v0__		  __v4
		==__v5__==
	*v8		--		 *v9
**/
void Scene::add_mark_room(const glm::dvec3& origin, double radius, shared_ptr<Material> m, shared_ptr<Material> wall_1, shared_ptr<Material> wall_2, shared_ptr<Material> wall_3, shared_ptr<Material> wall_4, shared_ptr<Material> wall_5, shared_ptr<Material> wall_6)
{

	// Floor
	world.add(std::make_shared<Plane>(glm::dvec3(0, -radius, 0), glm::dvec3(0, 1, 0), m));
	// Roof
	world.add(std::make_shared<Plane>(glm::dvec3(0, radius, 0), glm::dvec3(0, -1, 0), m));

	glm::dvec3 o(origin);
	glm::dquat rotQuat;
	rotQuat = glm::dquat(glm::radians(glm::dvec3(0,60,0)));

	//v0 = rotQuad * glm::vec3(v0);
	glm::dvec3 wallPos = origin - glm::dvec3(-radius,0,0);
	glm::dvec3 wallNormal;

	// Wall 1
	wallNormal = origin - wallPos;
	world.add(std::make_shared<Plane>(wallPos, wallNormal, wall_1));
	wallPos = o + rotQuat * (wallPos - o);
	wallNormal = origin - wallPos;
	world.add(std::make_shared<Plane>(wallPos, wallNormal, wall_2));
	wallPos = o + rotQuat * (wallPos - o);
	wallNormal = origin - wallPos;
	world.add(std::make_shared<Plane>(wallPos, wallNormal, wall_3));
	wallPos = o + rotQuat * (wallPos - o);
	wallNormal = origin - wallPos;
	world.add(std::make_shared<Plane>(wallPos, wallNormal, wall_4));
	wallPos = o + rotQuat * (wallPos - o);
	wallNormal = origin - wallPos;
	world.add(std::make_shared<Plane>(wallPos, wallNormal, wall_5));
	wallPos = o + rotQuat * (wallPos - o);
	wallNormal = origin - wallPos;
	world.add(std::make_shared<Plane>(wallPos, wallNormal, wall_6));
	
}


//void Scene::add_mark_room(const glm::dvec3& origin, double radius, shared_ptr<Material> m, shared_ptr<Material> wall_1, shared_ptr<Material> wall_2, shared_ptr<Material> wall_3, shared_ptr<Material> wall_4, shared_ptr<Material> wall_5, shared_ptr<Material> wall_6)
//{
//
//	double x1 = origin.x;
//	double x2 = origin.x - radius;
//	double x3 = origin.x + radius;
//
//	double z1 = origin.z - radius;
//	double z2 = origin.z + radius;
//	double z3 = origin.z - radius/2;
//	double z4 = origin.z + radius/2;
//	
//	// Wall corner vertices
//	glm::dvec3 v0{x2, origin.y, z4};
//	glm::dvec3 v1{x2, origin.y, z3};
//	glm::dvec3 v2{x1, origin.y, z1};
//	glm::dvec3 v3{x3, origin.y, z3};
//	glm::dvec3 v4{x3, origin.y, z4};
//	glm::dvec3 v5{x1, origin.y, z2};
//	
//	// Floor and ceiling vertices
//	glm::dvec3 v6{ origin - glm::dvec3(-radius*3, 0, radius*3) };
//	glm::dvec3 v7{ origin - glm::dvec3(-radius*3, 0, -radius*3) };
//	glm::dvec3 v8{ origin - glm::dvec3(radius*3, 0, -radius*3) };
//	glm::dvec3 v9{ origin - glm::dvec3(radius*3, 0, radius*3) };
//	
//	// Upper and lower y
//	glm::dvec3 y{0, radius, 0}; //ToFloor
//
//	// Create quads, add or subtract y for ceiling or floor coordinates
//	add_quad(v9 - y, v8 - y, v6 - y, v7 - y, m); // Floor
//	add_quad(v9 + y, v8 + y, v6 + y, v7 + y, m); // Ceiling
//	add_quad(v0 - y, v1 - y, v0 + y, v1 + y, wall_1); // wall_1 - Left wall -> clockwise next
//	add_quad(v1 - y, v2 - y, v1 + y, v2 + y, wall_2); // wall_2
//	add_quad(v2 - y, v3 - y, v2 + y, v3 + y, wall_3); // wall_3
//	add_quad(v3 - y, v4 - y, v3 + y, v4 + y, wall_4); // wall_4
//	add_quad(v4 - y, v5 - y, v4 + y, v5 + y, wall_5); // wall_5
//	add_quad(v5 - y, v0 - y, v5 + y, v0 + y, wall_6); // wall_6
//
//}

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

void Scene::add_cube_map(const std::string& top, const std::string& bottom, const std::string& left, const std::string& right, const std::string& front, const std::string& back)
{
	auto back_t = make_shared<ImageTexture>(back);
	auto bottom_t = make_shared<ImageTexture>(bottom);
	auto front_t = make_shared<ImageTexture>(front);
	auto left_t = make_shared<ImageTexture>(left);
	auto right_t = make_shared<ImageTexture>(right);
	auto top_t = make_shared<ImageTexture>(top);

	auto back_mat = make_shared<Unlit>(glm::dvec3(1, 1, 1));
	auto bottom_mat = make_shared<Unlit>(glm::dvec3(1, 1, 1));
	auto front_mat = make_shared<Unlit>(glm::dvec3(1, 1, 1));
	auto left_mat = make_shared<Unlit>(glm::dvec3(1, 1, 1));
	auto right_mat = make_shared<Unlit>(glm::dvec3(1, 1, 1));
	auto top_mat = make_shared<Unlit>(glm::dvec3(1, 1, 1));

	back_mat->texture = back_t;
	bottom_mat->texture = bottom_t;
	front_mat->texture = front_t;
	left_mat->texture = left_t;
	right_mat->texture = right_t;
	top_mat->texture = top_t;

	/* CREATE A SKYBOX */
	constexpr auto SIZE = 500;
	// Front
	world.add(std::make_shared<Quad>(glm::dvec3(-SIZE, SIZE, -SIZE), glm::dvec3(SIZE, SIZE, -SIZE), glm::dvec3(-SIZE, -SIZE, -SIZE), glm::dvec3(SIZE, -SIZE, -SIZE), front_mat));
	// Left
	world.add(std::make_shared<Quad>(glm::dvec3(-SIZE, SIZE, SIZE), glm::dvec3(-SIZE, SIZE, -SIZE), glm::dvec3(-SIZE, -SIZE, SIZE), glm::dvec3(-SIZE, -SIZE, -SIZE), left_mat));
	// Right
	world.add(std::make_shared<Quad>(glm::dvec3(SIZE, SIZE, -SIZE), glm::dvec3(SIZE, SIZE, SIZE), glm::dvec3(SIZE, -SIZE, -SIZE), glm::dvec3(SIZE, -SIZE, SIZE), right_mat));
	// Top
	world.add(std::make_shared<Quad>(glm::dvec3(-SIZE, SIZE, SIZE), glm::dvec3(SIZE, SIZE, SIZE), glm::dvec3(-SIZE, SIZE, -SIZE), glm::dvec3(SIZE, SIZE, -SIZE), top_mat));
	// Bottom
	world.add(std::make_shared<Quad>(glm::dvec3(-SIZE, -SIZE, -SIZE), glm::dvec3(SIZE, -SIZE, -SIZE), glm::dvec3(-SIZE, -SIZE, SIZE), glm::dvec3(SIZE, -SIZE, SIZE), bottom_mat));
	// Back
	world.add(std::make_shared<Quad>(glm::dvec3(SIZE, SIZE, SIZE), glm::dvec3(-SIZE, SIZE, SIZE), glm::dvec3(SIZE, -SIZE, SIZE), glm::dvec3(-SIZE, -SIZE, SIZE), back_mat));
}

void Scene::add_final_scene(){
	

	// Define Materials
	// Walls
	auto l_orange 	= make_shared<Lambertian>(color_255_to_01(243, 126, 65));
	auto l_pink   	= make_shared<Lambertian>(color_255_to_01(216, 36, 84));
	auto l_turk  	= make_shared<Lambertian>(color_255_to_01(0, 190, 178));
	auto l_blue   	= make_shared<Lambertian>(color_255_to_01(33, 73, 99));
	auto l_green 	= make_shared<Lambertian>(color_255_to_01(243, 126, 65));
	auto l_sage 	= make_shared<Lambertian>(color_255_to_01(44, 94, 46));

	//Floor & Ceiling
	auto walls_and_ceiling 	= make_shared<Lambertian>(glm::dvec3(1,1,1));

	// Objects
	//auto lambertian_object = make_shared<Lambertian>(color_255_to_01(44, 94, 46));;
	//auto glass = make_shared<Dielectric>(1.5);
	auto mirror = make_shared<Metal>(glm::dvec3(1.0, 1.0, 1.0), 0);
	//auto fuzzy_metal = make_shared<Metal>(glm::dvec3(1.0, 1.0, 1.0), 0.2);

	// Define Objects
	//add_cube(glm::dvec3(0.5, 0.5, -1.5), 0.2, fuzzy_metal, world, glm::dvec3(0, 20.0, 0));
	//add_cube(glm::dvec3(-0.4, -0.3, -0.8), 0.2, lambertian_object, world, glm::dvec3(0, 20.0, 0));
	//world.add(make_shared<Sphere>(glm::dvec3(-0.5, 0.0, -1.2), 0.35, mirror));
	//world.add(make_shared<Sphere>(glm::dvec3(-0.5, 0.0, -1.2), 0.35, glass));

	double eps = 1e-06;
	double y = 1 - eps;
	double z = -1;
	double size = 0.25;
	double x = 0;

	//add_cube(glm::dvec3(0.15, -0.5, -1.5), 0.2, lambertian_blue, world, glm::dvec3(0, 0, 0));
	std::vector<glm::dvec3> v = { glm::dvec3(x + size, y, z + size), glm::dvec3(x - size, y, z + size), glm::dvec3(x + size, y, z - size), glm::dvec3(x - size, y, z - size) };
	world.add(make_shared<Light>(v, glm::dvec3(x, y, z), 5.0, glm::dvec3{ 1, 1, 1 }));

	// Create_mark_room
	add_mark_room(glm::dvec3(0, 0, -1), 1, walls_and_ceiling, l_orange, l_pink, l_turk, l_blue, l_green, l_sage);
}




