#pragma once

#include"hittable_list.h"
#include"camera.h"
#include"light.h"
#include<string>

class Scene {

public:
	Scene();

	/* Methods */
	void setup_scene();
	void render_scene();
	void pixel_pass();
	void write_render_to_file(const std::string& image_name);

	/* Utility */
	glm::dvec3 ray_color(const Ray& ray, glm::dvec3 bg, const Hittable& world, int depth) const;
	
	// Adds a quad to the world
	void add_quad(const glm::dvec3& bottomLeft, const glm::dvec3& bottomRight, const glm::dvec3& topLeft, const glm::dvec3& topRight, shared_ptr<Material> m);

	// Adds a room to the world
	void add_room(const glm::dvec3& origin, double radius,
		shared_ptr<Material> m,
		shared_ptr<Material> left, shared_ptr<Material> right);

	// Adds a cube to the world
	void add_cube(const glm::dvec3& origin, double radius, shared_ptr<Material> m, HittableList& world_ref, glm::dvec3 rot);

private:
	HittableList world;
	Camera camera;
	glm::dvec3* framebuffer;
};