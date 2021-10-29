#include "scene.h"

int main() {

	Scene scene;

	// The 3 stages of ray-william-tracer
	//scene.setup_scene();
	//scene.pixel_pass();
	//scene.render_scene();

	scene.scene1();
	scene.render_scene();
	scene.write_render_to_file("scene1.ppm");

	// Factory render time
	//scene.sphere_unlit_scene();
	//scene.render_scene();
	//scene.write_render_to_file("sphere_unlit.ppm");

	//scene.sphere_diffuse_scene();
	//scene.render_scene();
	//scene.write_render_to_file("sphere_diffuse.ppm");

	//scene.scene_with_perfect_reflector();
	//scene.render_scene();
	//scene.write_render_to_file("sphere_perfect_reflector.ppm");
	//
	//scene.cornell_box_with_lamp_and_shadow();
	//scene.render_scene();
	//scene.write_render_to_file("cornell_box.ppm");

	/*scene.cornell_box_with_lamp_and_shadow_texture();
	scene.render_scene();
	scene.write_render_to_file("cornell_box_textured.ppm");*/
	//scene.view_render_in_SDL();
						
	// Now write our render to file "image.ppm"
	//scene.write_render_to_file("image.ppm");
	//scene.view_render_in_SDL();

	return 0;
}