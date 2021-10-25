#pragma once

#include<glm/vec3.hpp>

// As the name implies, take information from the framebuffer 
// + size of the screen from the camera and draw each pixel 
//
void render_framebuffer_as_SDL(const glm::dvec3* framebuffer, int buffer_width, int buffer_height, int samples_per_pixel);