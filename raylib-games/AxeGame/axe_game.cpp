#include "raylib.h"

int main()
{

    int width{800},height{450};
    InitWindow(width,height,"Axe Game");

    int circle_x{200},circle_y{200},radius{25};
    int axe_x{400},axe_y{0},direction{10};


    SetTargetFPS(60);
    while(!WindowShouldClose())                   //WindowShouldClose returns false by default
    {
        BeginDrawing();

        ClearBackground(WHITE);
        DrawCircle(circle_x,circle_y,radius,BLUE);
        DrawRectangle(axe_x,axe_y,50,50,BLUE);

        axe_y += direction;
        if(axe_y+50 > 450 || axe_y < 0)
        {
            direction = -direction;
        }

        if(IsKeyDown(KEY_D) && circle_x < (width-radius))
        {
            circle_x += 10;
        }
        if(IsKeyDown(KEY_A) && circle_x > (radius))
        {
            circle_x -= 10;
        }

        EndDrawing();
    }
}