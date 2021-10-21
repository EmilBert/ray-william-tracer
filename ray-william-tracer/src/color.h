#pragma once

#include<glm/vec3.hpp>
#include<iostream>

glm::ivec3 get_gamma_corrected_rgb_255(glm::dvec3 pixel_color, int samples_per_pixel);

void write_color(std::ostream& out, glm::dvec3 pixel_color, int samples_per_pixel);