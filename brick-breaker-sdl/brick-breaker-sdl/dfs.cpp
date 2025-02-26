#include <SDL2/SDL.h>
#include <vector>
#include <queue>
#include <iostream>
#include <cmath>
#include <chrono>

const int SCREEN_WIDTH = 600, SCREEN_HEIGHT = 600;
const int CELL_SIZE = 30;
const int ROWS = SCREEN_HEIGHT / CELL_SIZE, COLS = SCREEN_WIDTH / CELL_SIZE;

struct Node {
    int x, y;
    bool isWall = false, visited = false;
    float gCost = FLT_MAX, hCost = 0;
    Node* parent = nullptr;

    float TotalCost() const { return gCost + hCost; }
};

Node grid[ROWS][COLS];
Node* startNode = nullptr, * endNode = nullptr;
SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

int dx[4] = { 0, 0, -1, 1 };
int dy[4] = { -1, 1, 0, 0 };

bool initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return false;
    window = SDL_CreateWindow("Total War AI Pathfinding", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    return window && renderer;
}

void renderGrid() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            SDL_Rect cell = { j * CELL_SIZE, i * CELL_SIZE, CELL_SIZE, CELL_SIZE };
            if (&grid[i][j] == startNode) 
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            else if (&grid[i][j] == endNode) 
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            else if (grid[i][j].isWall)
                SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
            else if (grid[i][j].visited) 
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
            else 
                SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);

            SDL_RenderFillRect(renderer, &cell);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &cell);
        }
    }
    SDL_RenderPresent(renderer);
}

void handleMouseClick(int x, int y, bool leftClick) {
    int col = x / CELL_SIZE, row = y / CELL_SIZE;
    if (row >= 0 && row < ROWS && col >= 0 && col < COLS) {
        if (leftClick) endNode = &grid[row][col];
        else grid[row][col].isWall = !grid[row][col].isWall;
        renderGrid();
    }
}

void resetGrid() {
    for (auto& row : grid)
        for (auto& node : row) {
            node.visited = false;
            node.parent = nullptr;
            node.gCost = FLT_MAX;
            node.hCost = 0;
        }
}

bool dfs(Node* node) {
    if (!node || node->visited || node->isWall) return false;
    node->visited = true;

    renderGrid(); 
    SDL_Delay(30);

    if (node == endNode) return true;

    for (int i = 0; i < 4; i++) {
        int newX = node->x + dx[i], newY = node->y + dy[i];
        if (newX >= 0 && newX < COLS && newY >= 0 && newY < ROWS) {
            if (dfs(&grid[newY][newX])) return true;
        }
    }
    return false;
}

void bfs() {
    std::queue<Node*> q;
    q.push(startNode);
    startNode->visited = true;

    while (!q.empty()) {
        Node* node = q.front(); 
        q.pop();

        renderGrid(); 
        SDL_Delay(30);

        if (node == endNode) return;

        for (int i = 0; i < 4; i++) {
            int newX = node->x + dx[i], newY = node->y + dy[i];
            if (newX >= 0 && newX < COLS && newY >= 0 && newY < ROWS) {
                Node* neighbor = &grid[newY][newX];
                if (!neighbor->visited && !neighbor->isWall) {
                    neighbor->visited = true;
                    q.push(neighbor);
                }
            }
        }
    }
}

void dijkstra() {
    auto cmp = [](Node* a, Node* b) { 
        return a->gCost > b->gCost; 
        };
    std::priority_queue<Node*,std::vector<Node*>, decltype(cmp)> pq(cmp);

    startNode->gCost = 0;
    pq.push(startNode);

    while (!pq.empty()) {
        Node* node = pq.top(); pq.pop();
        node->visited = true;
        renderGrid();
        SDL_Delay(30);
        if (node == endNode) return;

        for (int i = 0; i < 4; i++) {
            int newX = node->x + dx[i], newY = node->y + dy[i];
            if (newX >= 0 && newX < COLS && newY >= 0 && newY < ROWS) {
                Node* neighbor = &grid[newY][newX];
                if (!neighbor->isWall) {
                    float newCost = node->gCost + 1;
                    if (newCost < neighbor->gCost) {
                        neighbor->gCost = newCost;
                        neighbor->parent = node;
                        pq.push(neighbor);
                    }
                }
            }
        }
    }
}

void aStar() {
    auto cmp = [](Node* a, Node* b) { 
        return a->TotalCost() > b->TotalCost(); 
        };
    std::priority_queue<Node*, std::vector<Node*>, decltype(cmp)> pq(cmp);

    startNode->gCost = 0;
    startNode->hCost = std::abs(startNode->x - endNode->x) + std::abs(startNode->y - endNode->y);
    pq.push(startNode);

    while (!pq.empty()) {
        Node* node = pq.top(); pq.pop();
        node->visited = true;
        renderGrid(); 
        SDL_Delay(30);
        if (node == endNode) return;

        for (int i = 0; i < 4; i++) {
            int newX = node->x + dx[i], newY = node->y + dy[i];
            if (newX >= 0 && newX < COLS && newY >= 0 && newY < ROWS) {
                Node* neighbor = &grid[newY][newX];
                if (!neighbor->isWall) {
                    float newCost = node->gCost + 1;
                    if (newCost < neighbor->gCost) {
                        neighbor->gCost = newCost;
                        neighbor->hCost = std::abs(neighbor->x - endNode->x) + std::abs(neighbor->y - endNode->y);
                        neighbor->parent = node;
                        pq.push(neighbor);
                    }
                }
            }
        }
    }
}

void runAlgorithms() {
    auto measure = [](auto algo, const char* name) {
        resetGrid();
        auto start = std::chrono::high_resolution_clock::now();
        algo();
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << name << " : " << std::chrono::duration<double>(end - start).count() << " seconds.\n";
        SDL_Delay(500);
        };

    measure([]() { dfs(startNode); }, "DFS Algo");
    measure([]() { bfs(); }, "BFS Algo");
    measure([]() { dijkstra(); }, "Dijkstra Algo");
    measure([]() { aStar(); }, "A* Algo");
}

int main(int argc, char* argv[]) {
    if (!initSDL()) return -1;
    for (int i = 0; i < ROWS; i++)
        for (int j = 0; j < COLS; j++)
            grid[i][j] = { j, i };

    startNode = &grid[0][0];
    endNode = &grid[ROWS - 1][COLS - 1];

    renderGrid();
    SDL_Event event;
    while (SDL_WaitEvent(&event)) {
        if (event.type == SDL_QUIT) break;
        if (event.type == SDL_MOUSEBUTTONDOWN) handleMouseClick(event.button.x, event.button.y, event.button.button == SDL_BUTTON_LEFT);
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) runAlgorithms();
    }
    return 0;
}
