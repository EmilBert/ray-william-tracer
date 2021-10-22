#pragma once

#include<string>
#include<glm/vec3.hpp>
#include<glm/vec2.hpp>

// Virtual class for texture types to implement

struct Texture {
	virtual glm::dvec3 get_pixel_value(const glm::dvec3& p, glm::dvec2 uv) = 0;
};

struct ImageTexture : Texture {
	ImageTexture(const std::string& file_name);

	glm::dvec3 get_pixel_value(const glm::dvec3& p, glm::dvec2 uv) override;

	// Members
	int width, height, bytes_per_pixel, bytes_per_line;
	unsigned char* image;
};

enum class TextureType {
	CHECKERED, // Classic checkered board texture
};

struct ProcederulTexture : Texture {
	// Used on objects that don't use UV mostly planes 
	ProcederulTexture(TextureType text_t) : texture_type(text_t) {}

	glm::dvec3 get_pixel_value(const glm::dvec3& p, glm::dvec2 uv) override;

	TextureType texture_type;
};