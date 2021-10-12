#include"scene.h"
#include<iostream>
#include<fstream>
#include"hittable.h"
#include"material.h"
#include"triangle.h"
#include"sphere.h"
#include"color.h"
#include"light.h"

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
	double eps = 1e-06;
	double y = 1 - eps;
	double z = -1;
	double size = 0.40;
	double x = 0;
	add_quad(glm::dvec3(x + size, y, z - size), glm::dvec3(x - size, y, z - size), glm::dvec3(x + size, y, z + size), glm::dvec3(x - size, y, z + size), diffuse_light);

	Light light({ glm::dvec3(x + size, y, z + size), glm::dvec3(x - size, y, z + size), glm::dvec3(x + size, y, z - size), glm::dvec3(x - size, y, z - size) });


	//world.add(make_shared<Triangle>(someData, glm::dvec3(0,0,0), 0, lambertian));
	//world.add(make_shared<Quad>(glm::dvec3(0, 0, -2), glm::dvec3(0, 2, -2), glm::dvec3(2, 0, -2), glm::dvec3(2, 2, -2), lambertian));

}

void Scene::render_scene()
{
	glm::dvec3 bg = { 0,0,0 };

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

			// Write the final, super-sampled color to our framebuffer
			// Dereference the memory adress and stride forward in memory
			*(pixel++) = pixel_color;
		}
	}
}


void Scene::pixel_pass()
{
	// Pass for now
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
		glm::dvec3 emitted = rec.mat_ptr->emitted(rec.p);

		if (!rec.mat_ptr->scatter(ray, rec, attenuation, scattered))
			return emitted;

		// Check if hit point is illuminated or shadowed?
		// List of all lights?
		if (!rec.mat_ptr->isLightSource()) {
			// Check if the point rec.p is in view of any light source we have
			// send a ray to all light sources
			/* 
			for object : world {
				if object is light source:
					send ray from rec.p to object.p, is there any obstruction? 
					use answer to determine if we are in shadow or not
			}
			*/
		}

		return emitted + attenuation * ray_color(scattered, bg, world, depth - 1);
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

	glm::dvec3 translation = { 0,0,0.5 };

	add_quad(v4, v0, v5, v1, left); // left
	add_quad(v0+translation, v3+translation, v1+translation, v2+translation, m); // front wall
	add_quad(v3, v7, v2, v6, right);  // h�ger v�ggen
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



