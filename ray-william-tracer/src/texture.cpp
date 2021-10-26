#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include<stb_image.h>

#include<iostream>
#include<glm/common.hpp>
#include<glm/trigonometric.hpp>

ImageTexture::ImageTexture(const std::string& file)
{
	image = stbi_load(file.c_str(), &width, &height, &bytes_per_pixel, 0);
	if (image == nullptr) {
		std::cout << "Failed to load image" << std::endl;
	}
	if (bytes_per_pixel != 3) {
		std::cout << "Warning, loading an image with 4 channels (not supported)" << std::endl;
	}
	bytes_per_line = bytes_per_pixel * width; // How many bytes on each horizontal line
}

glm::dvec3 ImageTexture::get_pixel_value(const glm::dvec3& p, glm::dvec2 uv)
{
	if (image == nullptr)
		return { 0,1,1 }; // Texture error color

	// Clamp input to [0,1] x [1,0]
	uv.x = glm::clamp(uv.x, 0.0, 1.0);
	uv.y = 1 - glm::clamp(uv.y, 0.0, 1.0); // Flip the v coord

	int i = static_cast<int>(uv.x * width); // Get our i,j position in the image, pixel coordinate
	int j = static_cast<int>(uv.y * height);

	// Clamp to be inside the image
	if (i >= width) i = width - 1;
	if (j >= height) j = height - 1;

	// The RGB is in [0,255] we scale down to [0.0, 1.0]
	static double color_scale = 1.0 / 255.0;

	// Final value
	unsigned char* pixel = image + j * bytes_per_line + i * bytes_per_pixel;
	return glm::dvec3(pixel[0], pixel[1], pixel[2]) * color_scale;
}

glm::dvec3 ProcederulTexture::get_pixel_value(const glm::dvec3& p, glm::dvec2 uv)
{
	glm::dvec3 pixel_value = { 0,0,0 };

	switch (texture_type) {
		// Do checkered texturing
	case TextureType::CHECKERED:
		auto sines = glm::sin(scale * p.x) * glm::sin(scale * p.y) * glm::sin(scale * p.z);
		if (sines < 0)
			pixel_value = glm::dvec3(0, 0, 0);
		else
			pixel_value = glm::dvec3(1, 1, 1);
		break;
	}

	return pixel_value;
}
