#pragma once

#include"utility.h"

/*
Quad xyz
Vector<xyz> samplesPoints

Quad position & size
generate samplePoints
*/

class Camera {
public:
	Camera(glm::dvec3 lookfrom, glm::dvec3 lookat, glm::dvec3 up, double vfov, double aspect_ratio) {
		// Initalize camera stuff
		double theta = glm::radians(vfov);
		double h = glm::tan(theta / 2);
		double viewport_height = 2.0 * h;
		double viewport_width = aspect_ratio * viewport_height;
		double focal_length = 1.0;

		auto w = glm::normalize(lookfrom - lookat); // Viewing direction
		auto u = glm::normalize(glm::cross(up, w)); // Cross our viewing direction and up vector
		auto v = glm::cross(w, u);

		// Some screen constants
		image_width = 720; 
		image_height = static_cast<int>(image_width / aspect_ratio);
		samples_per_pixel = 300;

		// Positionals
		origin = lookfrom;
		horizontal = viewport_width * u;
		vertical = viewport_height * v;
		lower_left_corner = origin - horizontal / 2.0 - vertical / 2.0 - w;
	}

	Ray get_ray(double u, double v) const {
		return Ray(origin, lower_left_corner + u * horizontal + v * vertical - origin);
	}

public:
	double image_width;
	double image_height;
	double samples_per_pixel;

private:
	glm::dvec3 origin;
	glm::dvec3 lower_left_corner;
	glm::dvec3 horizontal, vertical;
};
