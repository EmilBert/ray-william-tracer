#include"sdl_rendering.h"

//#include"camera.h"

#include <SDL.h>
#undef main

void render_framebuffer_as_SDL(const glm::dvec3* framebuffer, int buffer_width, int buffer_height)
{
    SDL_Event event;
    SDL_Renderer* renderer;
    SDL_Window* window;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(buffer_width, buffer_height, 0, &window, &renderer);

    bool running = true;
    while (running) {
        if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
            running = false;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0); // Background color
        SDL_RenderClear(renderer); // Clear 

        int x_counter = 0;
        int y_counter = 0;
        for (int i = 0; i < buffer_width * buffer_height; i++) {
            if (x_counter == buffer_width) {
                x_counter = 0;
                y_counter++;
            }

            // Draw the pixel
            glm::dvec3 pixel_color = framebuffer[i];
            SDL_SetRenderDrawColor(renderer, pixel_color.r, pixel_color.g, pixel_color.b, 255);
            SDL_RenderDrawPoint(renderer, x_counter, y_counter);

            x_counter++;
        }

        SDL_RenderPresent(renderer);
    }

    // Clean-up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
