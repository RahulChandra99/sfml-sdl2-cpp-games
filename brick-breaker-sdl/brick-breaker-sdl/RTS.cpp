#include <SDL2/SDL.h>
#include <vector>
#include <memory>
#include <cmath>

const int GRID_SIZE = 50;

struct Unit {
    SDL_Rect rect; // Position and size
    bool selected = false;
    float x, y;
    float targetX = -1, targetY = -1;
    float speed = 2.0f;

    Unit(int startX, int startY) : x(startX), y(startY) {
        rect = { startX, startY, GRID_SIZE, GRID_SIZE };
    }

    void MoveTowardsTarget() {
        if (targetX != -1 && targetY != -1) {
            float dx = targetX - x;
            float dy = targetY - y;
            float distance = sqrt(dx * dx + dy * dy);

            if (distance > 1.0f) {
                x += (dx / distance) * speed;
                y += (dy / distance) * speed;
                rect.x = static_cast<int>(x);
                rect.y = static_cast<int>(y);
            }
            else {
                x = targetX;
                y = targetY;
                rect.x = static_cast<int>(x);
                rect.y = static_cast<int>(y);
                targetX = -1;
                targetY = -1;
            }
        }
    }
};

class SelectionManager {
public:
    SDL_Rect selectionBox = { 0, 0, 0, 0 };
    bool isSelecting = false;

    void StartSelection(int x, int y) {
        selectionBox = { x, y, 0, 0 };
        isSelecting = true;
    }

    void UpdateSelection(int x, int y) {
        selectionBox.w = x - selectionBox.x;
        selectionBox.h = y - selectionBox.y;
    }

    void EndSelection(std::vector<std::unique_ptr<Unit>>& units) {
        isSelecting = false;
        NormalizeRect(selectionBox);
        for (auto& unit : units) {
            unit->selected = SDL_HasIntersection(&selectionBox, &unit->rect);
        }
    }

    void Draw(SDL_Renderer* renderer) {
        if (isSelecting) {
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            SDL_RenderDrawRect(renderer, &selectionBox);
        }
    }
private:
    void NormalizeRect(SDL_Rect& rect) {
        if (rect.w < 0) {
            rect.x += rect.w;
            rect.w = -rect.w;
        }
        if (rect.h < 0) {
            rect.y += rect.h;
            rect.h = -rect.h;
        }
    }
};

class Game {
public:
    bool isRunning;
    std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> window;
    std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> renderer;
    std::vector<std::unique_ptr<Unit>> units;
    SelectionManager selectionManager;

    Game() : isRunning(false), window(nullptr, SDL_DestroyWindow), renderer(nullptr, SDL_DestroyRenderer) {}

    bool Init() {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) return false;
        window.reset(SDL_CreateWindow("Unit Selection", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN));
        renderer.reset(SDL_CreateRenderer(window.get(), -1, SDL_RENDERER_ACCELERATED));
        if (!window || !renderer) return false;

        // Create sample units
        for (int i = 0; i < 5; i++) {
            units.emplace_back(std::make_unique<Unit>(100 + i * GRID_SIZE, 100));
        }

        isRunning = true;
        return true;
    }

    void DrawGrid() {
        SDL_SetRenderDrawColor(renderer.get(), 50, 50, 50, 255);
        for (int i = 0; i < 800; i += GRID_SIZE) {
            SDL_RenderDrawLine(renderer.get(), i, 0, i, 600);
        }
        for (int j = 0; j < 600; j += GRID_SIZE) {
            SDL_RenderDrawLine(renderer.get(), 0, j, 800, j);
        }
    }

    void HandleEvents() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                isRunning = false;
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    selectionManager.StartSelection(event.button.x, event.button.y);
                }
                else if (event.button.button == SDL_BUTTON_RIGHT) {
                    MoveSelectedUnits(event.button.x, event.button.y);
                }
                break;
            case SDL_MOUSEMOTION:
                if (selectionManager.isSelecting) {
                    selectionManager.UpdateSelection(event.motion.x, event.motion.y);
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    selectionManager.EndSelection(units);
                }
                break;
            }
        }
    }

    void MoveSelectedUnits(int x, int y) {
        x = (x / GRID_SIZE) * GRID_SIZE;
        y = (y / GRID_SIZE) * GRID_SIZE;

        int offset = 0;
        for (auto& unit : units) {
            if (unit->selected) {
                unit->targetX = x + (offset % 3) * GRID_SIZE;
                unit->targetY = y + (offset / 3) * GRID_SIZE;
                offset++;
            }
        }
    }

    void Update() {
        for (auto& unit : units) {
            unit->MoveTowardsTarget();
        }
    }

    void Render() {
        SDL_SetRenderDrawColor(renderer.get(), 0, 0, 0, 255);
        SDL_RenderClear(renderer.get());

        DrawGrid();

        for (const auto& unit : units) {
            SDL_SetRenderDrawColor(renderer.get(), unit->selected ? 255 : 0, 0, unit->selected ? 0 : 255, 255);
            SDL_RenderFillRect(renderer.get(), &unit->rect);
        }

        selectionManager.Draw(renderer.get());

        SDL_RenderPresent(renderer.get());
    }

    void Clean() {
        SDL_Quit();
    }
};

//int main(int argc, char* argv[]) {
//    Game game;
//    if (!game.Init()) return -1;
//
//    while (game.isRunning) {
//        game.HandleEvents();
//        game.Update();
//        game.Render();
//    }
//
//    game.Clean();
//    return 0;
//}
