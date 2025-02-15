#include <SDL2/SDL.h>
#include <iostream>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

int main(int argc, char* argv[])
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Create a window
    SDL_Window* window = SDL_CreateWindow("Brick Breaker",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    SDL_Rect ballRect = { 20,30,20,30 };
    SDL_Surface* ball = SDL_LoadBMP("ball.bmp");
    SDL_Texture* balltexture = SDL_CreateTextureFromSurface(renderer, ball);
    SDL_RenderCopy(renderer, balltexture, NULL, &ballRect);
    SDL_RenderPresent(renderer);

    if (!window)
    {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    // Wait 5 seconds before closing
    SDL_Delay(5000);

    // Destroy window and quit SDL
    SDL_Quit();

    return 0;
}
