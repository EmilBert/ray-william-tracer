#include"sdl_rendering.h"

#include<glm/vec2.hpp>

#include <SDL.h>
#undef main

void render_framebuffer_as_SDL(const glm::dvec3* framebuffer, int buffer_width, int buffer_height)
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

        // Draw the pixel
        glm::dvec3 pixel_color = framebuffer[i];
        SDL_SetRenderDrawColor(renderer, pixel_color.r, pixel_color.g, pixel_color.b, 255);
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
