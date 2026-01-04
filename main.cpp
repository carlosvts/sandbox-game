#include <vector>

#include "utils.hpp"
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include <raygui.h> // rayguy is on my usr/local/include 

constexpr const int WIDTH = 900;
constexpr const int HEIGHT = 600;
constexpr const int SCALE = 15;
constexpr const int GRID_WIDTH = WIDTH / SCALE;
constexpr const int GRID_HEIGHT = HEIGHT / SCALE;
constexpr const int FPS = 60;
constexpr const int GUIPANEL_X = 10;
constexpr const int GUIPANEL_Y = 10;
constexpr const int GUIPANEL_WIDTH = 120;
constexpr const int GUIPANEL_HEIGHT = 220;
constexpr const int BUTTONPANEL_X = 20;
constexpr const int BUTTONPANEL_Y = GUIPANEL_Y + 35;
constexpr const int BUTTON_WIDTH = 100;
constexpr const int BUTTON_HEIGHT = 30;

int checkSelectedElement(int &currentElement)
{
    GuiPanel({GUIPANEL_X, GUIPANEL_Y, GUIPANEL_WIDTH, GUIPANEL_HEIGHT}, "Particles");
    if (GuiButton({BUTTONPANEL_X, BUTTONPANEL_Y, BUTTON_WIDTH, BUTTON_HEIGHT}, "SAND")) { return SAND; }
    if (GuiButton({BUTTONPANEL_X, BUTTONPANEL_Y + 35, BUTTON_WIDTH, BUTTON_HEIGHT},"WATER")) { return WATER; }
    if (GuiButton({BUTTONPANEL_X, BUTTONPANEL_Y + 35 * 2, BUTTON_WIDTH, BUTTON_HEIGHT}, "FIRE")) { return FIRE; }
    if (GuiButton({BUTTONPANEL_X, BUTTONPANEL_Y + 35 * 3, BUTTON_WIDTH, BUTTON_HEIGHT}, "IRON")) { return IRON; }
    if (GuiButton({BUTTONPANEL_X, BUTTONPANEL_Y   + 35 * 4, BUTTON_WIDTH, BUTTON_HEIGHT}, "VOID")) { return VOID; }
    return currentElement; 
}

void saveParticles(std::vector<Particle>&grid, int selectedElement)
{
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !CheckCollisionPointRec(GetMousePosition(), {GUIPANEL_X, GUIPANEL_Y, GUIPANEL_WIDTH,GUIPANEL_HEIGHT}))
    {
        int posX = GetMouseX() / SCALE;
        int posY = GetMouseY() / SCALE;
        if (posX >= GRID_WIDTH || posY >= GRID_HEIGHT || posX < 0 || posY < 0) { return; }
        int index = (posY * GRID_WIDTH) + posX;
        Particle particle;
        particle.exists = true;
        particle.rect = {static_cast<float>(posX * SCALE), static_cast<float>(posY * SCALE), SCALE, SCALE};
        switch (selectedElement)
        {
        case SAND:
            {
                particle.type = SAND;
                particle.color = YELLOW;
                break; 
            }
            case WATER:
            {
                particle.type = WATER;
                particle.color = BLUE;
                break;
            }
            case FIRE:
            {
                particle.type = FIRE;
                particle.color = RED;
                break;
            }
            case IRON:
            {
                particle.type = IRON;
                particle.color = GRAY;
                break;
            }
            default:
            {
                particle.type = VOID;
                particle.color = BLACK;
                break;
            }
        }
        grid[index] = particle;
    }
}

void drawParticles(std::vector<Particle> &grid)
{
    for (size_t i = 0; i < grid.size(); ++i)
    {
        if (grid.at(i).exists)
        {
            DrawRectangleRec(grid.at(i).rect, grid.at(i).color);
        }
    }
}

void calculatePhysics(std::vector<Particle> &grid)
{
    // reset update status
    for (auto& particle : grid) { particle.wasUpdated = false; }

    // calculate physics 
    for (int i = grid.size() - 1; i >= 0; --i)
    {
        Particle *self = &grid.at(i);
        if (self->type == SAND)
        {
            int index = i + GRID_WIDTH;
            if (index > static_cast<int>(grid.size())) { continue; }
            Particle *other = &grid.at(index);
            if ((other->type == VOID || !other->exists) && !other->wasUpdated) 
            {
                other->type = self->type;
                other->color = self->color;
                other->exists = true;
                other->rect.x = (index % GRID_WIDTH) * SCALE;
                other->rect.y = (index / GRID_WIDTH) * SCALE;
            
                self->wasUpdated = false;
                self->type = VOID;
                self->color = BLACK;
                self->exists = false; 
            }
        }
        else if (self->type == WATER)
        {

        }
        else if (self->type == FIRE)
        {

        }
        else if(self->type == IRON)
        {

        }
    }
}

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
        grid.at(i).acceleration = 0;
        grid.at(i).exists = true;
        grid.at(i).velocity = 0;
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
            DrawFPS(WIDTH - 200, 30);
            DrawText("Sandbox simulator", WIDTH - 200, 10, 21, GREEN);
 
            saveParticles(grid, selectedElement);
            if (simulationTimer >= maxTime)
            {
                calculatePhysics(grid);
                simulationTimer = 0.0f;
            }

            drawParticles(grid);
            selectedElement = checkSelectedElement(selectedElement); 

        EndDrawing();
    }
    CloseWindow();
    return 0;
}

