#include"color.h"

#include<glm/common.hpp>

glm::ivec3 get_gamma_corrected_rgb_255(glm::dvec3 pixel_color, int samples_per_pixel)
{
	glm::dvec3 color_255 = pixel_color;

	// Divide the color by the number of samples.
	// Applying "gamma 2", gamma correction through squaring r,g,b components i.e raising it to 1/2
	double scale = 1.0 / samples_per_pixel;
	color_255.r = sqrt(scale * color_255.r);
	color_255.g = sqrt(scale * color_255.g);
	color_255.b = sqrt(scale * color_255.b);

	return {
		static_cast<int>(256 * glm::clamp(color_255.r, 0.0, 0.999)),
		static_cast<int>(256 * glm::clamp(color_255.g, 0.0, 0.999)),
		static_cast<int>(256 * glm::clamp(color_255.b, 0.0, 0.999))
	};
}

void write_color(std::ostream& out, glm::dvec3 pixel_color, int samples_per_pixel)
{
	glm::ivec3 gamma_corrected_255 = get_gamma_corrected_rgb_255(pixel_color, samples_per_pixel);
	out << gamma_corrected_255.r << ' '
		<< gamma_corrected_255.g << ' '
		<< gamma_corrected_255.b << '\n';
}
