#pragma once //compiler directive that ensures the file is included only once in compilation unit and helps in reducing preprocessor overhead

#ifndef GAME_H //include guard using pre processor macros
#define GAME_H

/*
	Both #pragma once and #ifndef GAME_H / #define GAME_H serve the same purpose: preventing multiple inclusions of a header file in a C++ program.
	#2nd version works on all compilers while other might not
*/


#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <vector>
#include <memory>
#include "Ball.h"
#include "Paddle.h"
#include "Brick.h"

constexpr int WINDOW_WIDTH = 800; 
constexpr int WINDOW_HEIGHT = 600;

/*
	constexpr is a compile time constant specifier, more optimized version of constant, it guarantees compile time evaluation while const can still allow runtime evaluations.
*/

class BBGame{
public:
    BBGame();
    bool init();
    void run();
    void cleanup();

private:
    void handleEvents();
    void update();
    void render();

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)> bgTexture{ nullptr, SDL_DestroyTexture };

    Ball ball;
    Paddle paddle;
    std::vector<Brick> bricks;
    bool running;
};


#endif // !GAME_H

