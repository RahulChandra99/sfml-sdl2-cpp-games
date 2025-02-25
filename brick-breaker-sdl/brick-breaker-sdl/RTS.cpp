#include <SDL2/SDL.h>
#include <vector>

struct Unit {
    SDL_Rect rect; // Position and size
    bool selected = false;
    int targetX = -1, targetY = -1;

    void MoveTowardsTarget() {
        if (targetX != -1 && targetY != -1) {
            int speed = 2;
            if (rect.x < targetX) rect.x += speed;
            else if (rect.x > targetX) rect.x -= speed;
            if (rect.y < targetY) rect.y += speed;
            else if (rect.y > targetY) rect.y -= speed;

            if (abs(rect.x - targetX) < speed && abs(rect.y - targetY) < speed) {
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

    void EndSelection(std::vector<Unit>& units) {
        isSelecting = false;
        NormalizeRect(selectionBox);
        for (auto& unit : units) {
            unit.selected = SDL_HasIntersection(&selectionBox, &unit.rect);
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
    SDL_Window* window;
    SDL_Renderer* renderer;
    std::vector<Unit> units;
    SelectionManager selectionManager;

    Game() : isRunning(false), window(nullptr), renderer(nullptr) {}

    bool Init() {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) return false;
        window = SDL_CreateWindow("Unit Selection", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!window || !renderer) return false;

        // Create sample units
        for (int i = 0; i < 5; i++) {
            units.push_back({ {100 + i * 60, 100, 40, 40} });
        }

        isRunning = true;
        return true;
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
        for (auto& unit : units) {
            if (unit.selected) {
                unit.targetX = x;
                unit.targetY = y;
            }
        }
    }

    void Update() {
        for (auto& unit : units) {
            unit.MoveTowardsTarget();
        }
    }

    void Render() {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        for (const auto& unit : units) {
            SDL_SetRenderDrawColor(renderer, unit.selected ? 255 : 0, 0, unit.selected ? 0 : 255, 255);
            SDL_RenderFillRect(renderer, &unit.rect);
        }

        selectionManager.Draw(renderer);

        SDL_RenderPresent(renderer);
    }

    void Clean() {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
};

int main(int argc, char* argv[]) {
    Game game;
    if (!game.Init()) return -1;

    while (game.isRunning) {
        game.HandleEvents();
        game.Update();
        game.Render();
    }

    game.Clean();
    return 0;
}
