#pragma once

#include<glm/vec3.hpp>
#include<iostream>

void write_color(std::ostream& out, glm::dvec3 pixel_color, int samples_per_pixel) {
	double r = pixel_color.x;
	double g = pixel_color.y;
	double b = pixel_color.z;

	// Divide the color by the number of samples.
	// Applying "gamma 2", gamma correction through squaring r,g,b components i.e raising it to 1/2
	double scale = 1.0 / samples_per_pixel;
    r = sqrt(scale * r);
    g = sqrt(scale * g);
    b = sqrt(scale * b);

	out << static_cast<int>(256 * clamp(r, 0.0, 0.999)) << ' '
		<< static_cast<int>(256 * clamp(g, 0.0, 0.999)) << ' '
		<< static_cast<int>(256 * clamp(b, 0.0, 0.999)) << '\n';
}