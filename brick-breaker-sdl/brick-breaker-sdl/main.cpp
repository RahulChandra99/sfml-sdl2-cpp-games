#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>
#include <memory>

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;

// Ball Properties
constexpr int BALL_WIDTH = 20;
constexpr int BALL_HEIGHT = 20;
constexpr int BALL_SPEED = 3;

// Paddle Properties
constexpr int PADDLE_WIDTH = 130;
constexpr int PADDLE_HEIGHT = 12;
constexpr int PADDLE_SPEED = 6;

// Brick Properties
constexpr int BRICK_WIDTH = 70;
constexpr int BRICK_HEIGHT = 25;
constexpr int BRICK_ROWS = 5;
constexpr int BRICK_COLUMNS = 10;

struct Brick {
    int x, y;
    bool active = true;
};

class Game {
public:
    Game() : running(true), moveLeft(false), moveRight(false),
        ballX(WINDOW_WIDTH / 2), ballY(WINDOW_HEIGHT / 2),
        ballVelX(BALL_SPEED), ballVelY(BALL_SPEED),
        paddleX(WINDOW_WIDTH / 2 - PADDLE_WIDTH / 2) {}

    bool init();
    void run();
    void cleanup();

private:
    void handleEvents();
    void update();
    void render();
    void checkCollisions();

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)> ballTexture{ nullptr, SDL_DestroyTexture };
    std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)> paddleTexture{ nullptr, SDL_DestroyTexture };
    std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)> bgTexture{ nullptr, SDL_DestroyTexture };

    bool running;
    bool moveLeft, moveRight;
    int ballX, ballY, ballVelX, ballVelY;
    int paddleX;
    std::vector<Brick> bricks;
};

bool Game::init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! Error: " << SDL_GetError() << std::endl;
        return false;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "SDL_image could not initialize! Error: " << IMG_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("Brick Breaker",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window could not be created! Error: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! Error: " << SDL_GetError() << std::endl;
        return false;
    }

    auto loadTexture = [this](const std::string& path) -> std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)> {
        SDL_Surface* surface = IMG_Load(path.c_str());
        if (!surface) {
            std::cerr << "Failed to load image: " << path << " Error: " << IMG_GetError() << std::endl;
            return { nullptr, SDL_DestroyTexture };
        }
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        return { texture, SDL_DestroyTexture };
        };

    ballTexture = loadTexture("Ball Red.png");
    paddleTexture = loadTexture("Paddle Blue.png");
    bgTexture = loadTexture("bg.jpg");

    // Initialize Bricks
    for (int i = 0; i < BRICK_ROWS; i++) {
        for (int j = 0; j < BRICK_COLUMNS; j++) {
            bricks.push_back({ j * (BRICK_WIDTH + 10), i * (BRICK_HEIGHT + 10) });
        }
    }

    return true;
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }
        else if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_LEFT) moveLeft = true;
            if (event.key.keysym.sym == SDLK_RIGHT) moveRight = true;
        }
        else if (event.type == SDL_KEYUP) {
            if (event.key.keysym.sym == SDLK_LEFT) moveLeft = false;
            if (event.key.keysym.sym == SDLK_RIGHT) moveRight = false;
        }
    }
}

void Game::update() {
    if (moveLeft && paddleX > 0) {
        paddleX -= PADDLE_SPEED;
    }
    if (moveRight && paddleX + PADDLE_WIDTH < WINDOW_WIDTH) {
        paddleX += PADDLE_SPEED;
    }

    ballX += ballVelX;
    ballY += ballVelY;

    // Ball Collision with Walls
    if (ballX <= 0 || ballX + BALL_WIDTH >= WINDOW_WIDTH) {
        ballVelX = -ballVelX;
    }
    if (ballY <= 0) {
        ballVelY = -ballVelY;
    }

    // Ball Collision with Paddle
    if (ballY + BALL_HEIGHT >= WINDOW_HEIGHT - 30 &&
        ballX + BALL_WIDTH >= paddleX &&
        ballX <= paddleX + PADDLE_WIDTH) {

        ballVelY = -ballVelY;
        int paddleCenter = paddleX + PADDLE_WIDTH / 2;
        int ballCenter = ballX + BALL_WIDTH / 2;

        if (ballCenter < paddleCenter - 30) ballVelX = -BALL_SPEED;
        else if (ballCenter > paddleCenter + 30) ballVelX = BALL_SPEED;
    }

    // Ball Collision with Bricks
    for (auto& brick : bricks) {
        if (brick.active &&
            ballX + BALL_WIDTH >= brick.x &&
            ballX <= brick.x + BRICK_WIDTH &&
            ballY + BALL_HEIGHT >= brick.y &&
            ballY <= brick.y + BRICK_HEIGHT) {

            ballVelY = -ballVelY;
            brick.active = false;
            break;
        }
    }

    // Lose Condition
    if (ballY + BALL_HEIGHT >= WINDOW_HEIGHT) {
        std::cout << "Game Over!\n";
        running = false;
    }
}

void Game::render() {
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, bgTexture.get(), nullptr, nullptr);

    SDL_Rect ballRect = { ballX, ballY, BALL_WIDTH, BALL_HEIGHT };
    SDL_RenderCopy(renderer, ballTexture.get(), nullptr, &ballRect);

    SDL_Rect paddleRect = { paddleX, WINDOW_HEIGHT - 30, PADDLE_WIDTH, PADDLE_HEIGHT };
    SDL_RenderCopy(renderer, paddleTexture.get(), nullptr, &paddleRect);

    for (const auto& brick : bricks) {
        if (brick.active) {
            SDL_Rect brickRect = { brick.x, brick.y, BRICK_WIDTH, BRICK_HEIGHT };
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderFillRect(renderer, &brickRect);
        }
    }

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

int main(int argc, char ** argv) {
    Game game;
    if (!game.init()) return -1;
    game.run();
    game.cleanup();
    return 0;
}
