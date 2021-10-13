#include "scene.h"

int main() {

	Scene scene;

	// The 3 stages of ray-william-tracer
	scene.setup_scene();
	scene.pixel_pass();
	scene.render_scene();

	// Now write our render to file "image.ppm"
	//scene.write_render_to_file("image.ppm");
	scene.view_render_in_SDL();

	return 0;
}