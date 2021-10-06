#pragma once

#include"utility.h"

class Camera {
public:
	Camera() {
		double aspect_ratio = 1;
		double viewport_height = 2.0;
		double viewport_width = aspect_ratio * viewport_height;
		double focal_length = 1.0;

		origin = glm::dvec3(0, 0, 0);
		horizontal = glm::dvec3(viewport_width, 0, 0);
		vertical = glm::dvec3(0, viewport_height, 0);
		lower_left_corner = origin - horizontal / 2.0 - vertical / 2.0 - glm::dvec3(0, 0, focal_length);
	}

	Ray get_ray(double u, double v) const {
		return Ray(origin, glm::normalize(lower_left_corner + u * horizontal + v * vertical - origin));
	}

private:
	glm::dvec3 origin;
	glm::dvec3 lower_left_corner;
	glm::dvec3 horizontal, vertical;
};