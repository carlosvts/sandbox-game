#include "game.hpp"
int main()
{
    InitWindow(WIDTH, HEIGHT, "Sandbox");
    SetTargetFPS(FPS);
    ShowCursor();

    std::vector<Particle> grid;
    grid.reserve(GRID_WIDTH * GRID_HEIGHT);
    grid.resize(GRID_WIDTH * GRID_HEIGHT);
    // filling grid
    for (int i = 0; i < GRID_WIDTH * GRID_HEIGHT; ++i)
    {
        grid.at(i).type = VOID;

        int posX = i % GRID_WIDTH;
        int posY = i / GRID_WIDTH;
        grid.at(i).rect.x = posX * SCALE;
        grid.at(i).rect.y = posY * SCALE;
        grid.at(i).rect.width = SCALE;
        grid.at(i).rect.height = SCALE;
        grid.at(i).exists = true;
        grid.at(i).wasUpdated = false;
        grid.at(i).color = BLACK;
    }

    int selectedElement = 0;
    float simulationTimer = 0.0f;
    const float maxTime = 0.06f;

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();
        simulationTimer += dt;

        BeginDrawing();
            ClearBackground(BLACK);
 
            saveParticles(grid, selectedElement);
            if (simulationTimer >= maxTime)
            {
                calculatePhysics(grid);
                simulationTimer = 0.0f;
            }

            drawParticles(grid);
            DrawFPS(WIDTH - 200, 30);
            DrawText("Carlosvts's Sandbox", WIDTH - 230, 10, 21, GREEN);
            selectedElement = checkSelectedElement(selectedElement); 

        EndDrawing();
    }
    CloseWindow();
    return 0;
}

