#include <SDL2/SDL.h>
#include <vector>
#include <stack>
#include <iostream>

const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 600;
const int CELL_SIZE = 30;
const int ROWS = SCREEN_HEIGHT / CELL_SIZE;
const int COLS = SCREEN_WIDTH / CELL_SIZE;

struct Node {
    int x, y;
    bool isWall = false;
    bool visited = false;
};

// Grid and DFS stack
Node grid[ROWS][COLS];
std::stack<Node*> dfsStack;
std::vector<Node*> path;

// Start and End Points
Node* startNode = nullptr;
Node* endNode = nullptr;

// SDL Variables
SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
bool dfsStarted = false;

// Directions for DFS (Up, Down, Left, Right)
int dirX[4] = { 0, 0, -1, 1 };
int dirY[4] = { -1, 1, 0, 0 };

// Initialize SDL
bool initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL Init Failed: " << SDL_GetError() << std::endl;
        return false;
    }
    window = SDL_CreateWindow("DFS Pathfinding", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window Creation Failed: " << SDL_GetError() << std::endl;
        return false;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer Creation Failed: " << SDL_GetError() << std::endl;
        return false;
    }
    return true;
}

// Reset only the visited nodes (Keeps walls and custom end point intact)
void clearVisitedNodes() {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            grid[i][j].visited = false;
        }
    }
    dfsStack = std::stack<Node*>(); // Clear the DFS stack
    path.clear(); // Clear the path visualization
}

// Render the grid and path
void renderGrid() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            SDL_Rect cell = { j * CELL_SIZE, i * CELL_SIZE, CELL_SIZE, CELL_SIZE };

            if (&grid[i][j] == startNode)
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Start - Green
            else if (&grid[i][j] == endNode)
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // End - Red
            else if (grid[i][j].isWall)
                SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255); // Walls - Dark Gray
            else if (grid[i][j].visited)
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Visited - Blue
            else
                SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255); // Unvisited - Gray

            SDL_RenderFillRect(renderer, &cell);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &cell);
        }
    }

    // Render the final path
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Yellow for the path
    for (Node* node : path) {
        SDL_Rect cell = { node->x * CELL_SIZE, node->y * CELL_SIZE, CELL_SIZE, CELL_SIZE };
        SDL_RenderFillRect(renderer, &cell);
    }

    SDL_RenderPresent(renderer);
}

// Depth-First Search Algorithm (Modified to be called multiple times)
bool dfs(Node* node) {
    if (node == nullptr || node->visited || node->isWall) return false;

    node->visited = true;
    dfsStack.push(node);
    renderGrid();
    SDL_Delay(50); // Delay to visualize the search process

    if (node == endNode) {
        path.clear();
        while (!dfsStack.empty()) {
            path.push_back(dfsStack.top());
            dfsStack.pop();
        }
        return true;
    }

    for (int i = 0; i < 4; i++) {
        int newX = node->x + dirX[i];
        int newY = node->y + dirY[i];

        if (newX >= 0 && newX < COLS && newY >= 0 && newY < ROWS) {
            if (dfs(&grid[newY][newX])) return true;
        }
    }

    dfsStack.pop();
    return false;
}

// Handle Mouse Input
void handleMouseClick(int x, int y, bool leftClick) {
    int col = x / CELL_SIZE;
    int row = y / CELL_SIZE;

    if (row >= 0 && row < ROWS && col >= 0 && col < COLS) {
        if (leftClick) {
            endNode = &grid[row][col]; // Set custom end point
        }
        else {
            grid[row][col].isWall = !grid[row][col].isWall; // Toggle walls
        }
        renderGrid();
    }
}

// Main loop
void gameLoop() {
    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    handleMouseClick(event.button.x, event.button.y, true); // Left-click: Set endpoint
                }
                else if (event.button.button == SDL_BUTTON_RIGHT) {
                    handleMouseClick(event.button.x, event.button.y, false); // Right-click: Set walls
                }
            }
            else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_SPACE) {
                    clearVisitedNodes(); // Clear previous DFS path but keep walls and end node
                    dfsStarted = true;
                    dfs(startNode); // Start DFS search
                }
            }
        }
    }
}

// Cleanup SDL
void cleanup() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

//// Main
//int main(int argc, char* argv[]) {
//    if (!initSDL()) return -1;
//
//    // Initialize grid and render first time
//    for (int i = 0; i < ROWS; i++) {
//        for (int j = 0; j < COLS; j++) {
//            grid[i][j] = { j, i, false, false };
//        }
//    }
//    startNode = &grid[0][0];  // Set fixed start point
//    endNode = &grid[ROWS - 1][COLS - 1]; // Default end point
//
//    renderGrid(); // Draw the initial grid
//
//    gameLoop();
//    cleanup();
//    return 0;
//}
