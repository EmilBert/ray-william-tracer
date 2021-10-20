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

	/* Render with SDL */
	void view_render_in_SDL() const;

	/* Utility */
	glm::dvec3 trace_ray(const Ray& ray, const Hittable& world, int depth) const;
	
	/* Returns the factor G for hit_record */
	double light_ray_pass(const hit_record& rec) const;

	// Adds a quad to the world
	void add_quad(const glm::dvec3& bottomLeft, const glm::dvec3& bottomRight, const glm::dvec3& topLeft, const glm::dvec3& topRight, shared_ptr<Material> m);

	// Adds a room to the world
	void add_cornell_box(const glm::dvec3& origin, double radius,
		shared_ptr<Material> m,
		shared_ptr<Material> left,
		shared_ptr<Material> right);

	void add_mark_room(const glm::dvec3& origin, double radius,
		shared_ptr<Material> m,
		shared_ptr<Material> wall_1,
		shared_ptr<Material> wall_2,
		shared_ptr<Material> wall_3,
		shared_ptr<Material> wall_4,
		shared_ptr<Material> wall_5,
		shared_ptr<Material> wall_6);

	// Adds a cube to the world
	void add_cube(const glm::dvec3& origin, double radius, shared_ptr<Material> m, HittableList& world_ref, glm::dvec3 rot);

public:
	HittableList world;
	Camera camera;
	int min_depth = 4;
	int max_depth = 8;
	glm::dvec3* framebuffer;
	glm::dvec3 bg = { 0,0,0 }; 

};
