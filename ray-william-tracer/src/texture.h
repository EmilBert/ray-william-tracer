#pragma once

#include<glm/vec3.hpp>
#include<glm/vec2.hpp>

struct Texture {
	Texture();

	~Texture() {
		free(image); // Free the loaded block of memory
	}

	glm::dvec3 get_pixel_value(glm::dvec2 uv);

	// Members
	int width, height, bytes_per_pixel, bytes_per_line;
	unsigned char* image;
};