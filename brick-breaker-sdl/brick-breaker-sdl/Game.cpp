#include "Game.h"

BBGame::BBGame() : running(true), ball(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2), paddle(WINDOW_WIDTH / 2 - PADDLE_WIDTH / 2) {}

bool Game::init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return false;
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) return false;

    window = SDL_CreateWindow("Brick Breaker", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) return false;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) return false;

    SDL_Surface* surface = IMG_Load("assets/bg.jpg");
    if (!surface) return false;
    bgTexture.reset(SDL_CreateTextureFromSurface(renderer, surface));
    SDL_FreeSurface(surface);

    // Initialize bricks
    for (int i = 0; i < BRICK_ROWS; i++) {
        for (int j = 0; j < BRICK_COLUMNS; j++) {
            bricks.push_back(Brick(j * (BRICK_WIDTH + 10), i * (BRICK_HEIGHT + 10)));
        }
    }

    return true;
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) running = false;
        paddle.handleInput(event);
    }
}

void Game::update() {
    ball.move();
    paddle.update();
    ball.checkCollisions(paddle, bricks);
}

void Game::render() {
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, bgTexture.get(), nullptr, nullptr);

    ball.render(renderer);
    paddle.render(renderer);

    for (auto& brick : bricks) brick.render(renderer);

    SDL_RenderPresent(renderer);
}

void Game::run() {
    while (running) {
        handleEvents();
        update();
        render();
        SDL_Delay(16);
    }
}

void Game::cleanup() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}
