#pragma once

#include<glm/vec3.hpp>
#include<glm/vec2.hpp>

enum class TextureType {
	CHECKERED, // Classic checkered board texture
};

// Virtual class for texture types to implement

struct Texture {
	virtual glm::dvec3 get_pixel_value(const glm::dvec3& p, glm::dvec2 uv) = 0;
};

struct ImageTexture : Texture {
	ImageTexture(const char* file_name);

	glm::dvec3 get_pixel_value(const glm::dvec3& p, glm::dvec2 uv) override;

	// Members
	int width, height, bytes_per_pixel, bytes_per_line;
	unsigned char* image;
};

struct ProcederulTexture : Texture {
	// Used on objects that don't use UV mostly planes 
	ProcederulTexture(TextureType text_t, double scale) : texture_type(text_t), scale(scale) {}

	glm::dvec3 get_pixel_value(const glm::dvec3& p, glm::dvec2 uv) override;

	TextureType texture_type;
	double scale;
};