#include"sdl_rendering.h"

#include"color.h"
#include<glm/vec2.hpp>

#include <SDL.h>
#undef main

void render_framebuffer_as_SDL(const glm::dvec3* framebuffer, int buffer_width, int buffer_height, int samples_per_pixel)
{
    SDL_Event event;
    SDL_Renderer* renderer;
    SDL_Window* window;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(buffer_width, buffer_height, 0, &window, &renderer);
 
    /* Do the render */
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0); // Background color
    SDL_RenderClear(renderer); // Clear 

    glm::ivec2 iP = { 0,0 };
    for (int i = 0; i < buffer_width * buffer_height; i++) {
        if (iP.x == buffer_width) {
            iP.x = 0;
            iP.y++;
        }

        glm::dvec3 pixel_color = framebuffer[i];
        // Convert to correct using the correction function from color
        auto pixel_color_255 = get_gamma_corrected_rgb_255(pixel_color, samples_per_pixel);
        // Draw the pixel
        SDL_SetRenderDrawColor(renderer, pixel_color_255.r, pixel_color_255.g, pixel_color_255.b, 255);
        SDL_RenderDrawPoint(renderer, iP.x, iP.y);

        iP.x++;
    }

    bool running = true;
    while (running) {
        if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
            running = false;
        SDL_RenderPresent(renderer);
    }

    // Clean-up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
