#include <ios>
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
    if (GuiButton({BUTTONPANEL_X, BUTTONPANEL_Y + 35 * 4, BUTTON_WIDTH, BUTTON_HEIGHT}, "GAS")) { return GAS; }
    if (GuiButton({BUTTONPANEL_X, BUTTONPANEL_Y   + 35 * 5, BUTTON_WIDTH, BUTTON_HEIGHT}, "VOID")) { return VOID; }
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
            case GAS:
            {
                particle.type = GAS;
                particle.color = SKYBLUE;
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
    for (Particle& particle : grid) { particle.wasUpdated = false; }

    // calculate physics 
    for (int i = grid.size() - 1; i >= 0; --i)
    {
        Particle *self = &grid.at(i);
        
        if (self->wasUpdated) { continue; }

        if (self->type == SAND)
        {
            int index = i + GRID_WIDTH;
            if (index > static_cast<int>(grid.size() - 1)) { continue; }
            Particle *other = &grid.at(index);
            if ((other->type == VOID || other->type == WATER) && !other->wasUpdated) 
            {
                // if down is VOID
                if (other->type == VOID)
                {
                    other->type = SAND;
                    other->color = self->color;
                    other->exists = true;
                    other->rect.x = (index % GRID_WIDTH) * SCALE;
                    other->rect.y = (index / GRID_WIDTH) * SCALE;
                    other->wasUpdated = true; 

                    self->type = VOID;
                    self->color = BLACK;
                    self->exists = false;
                    self->wasUpdated = true;
                }
                else if(other->type == WATER)
                {
                    other->type = SAND;
                    other->color = self->color;
                    other->wasUpdated = true;

                    self->type = WATER;
                    self->color = BLUE;
                    self->exists = true;
                    self->wasUpdated = true;
                }
            }
        }
        else if (self->type == WATER)
        {
            int index = i + GRID_WIDTH;
            // left, down, right 
            int leftIndex = i - 1;
            int rightIndex = i + 1;
            if (leftIndex <= 0 || rightIndex >= static_cast<int>(grid.size()) || i <= 0 || i >= static_cast<int>(grid.size())) { continue; }
            // std::vector<Particle*> others = {&grid.at(leftIndex), &grid.at(index), &grid.at(rightIndex)};
           
            if (index < static_cast<int>(grid.size()))
            {
                Particle* under = &grid.at(index);
            
                // if the under is void, just go down again
                if ((under->type == VOID || !under->exists) && !under->wasUpdated)
                {
                    under->type = self->type;
                    under->color = self->color;
                    under->exists = true;
                    under->rect.x= (index % GRID_WIDTH) * SCALE;
                    under->rect.y = (index / GRID_WIDTH) * SCALE;

                    self->wasUpdated = true;
                    self->type = VOID;
                    self->color = BLACK;
                    self->exists = false;
                    continue;

                }

                // if the under is iron, swipes horizontally
                else if (under->type == IRON || under->type == WATER || under->type == SAND || under->type == GAS)
                {
                    // variable to make more random where water will slip 
                    Particle* left = &grid.at(leftIndex);
                    Particle* right = &grid.at(rightIndex);
                    bool canGoLeft = (i % GRID_WIDTH > 0);
                    bool canGoRight = (i % GRID_WIDTH < GRID_WIDTH - 1);

                    int randomness = GetRandomValue(0, 1);
                    for(int attempt = 0; attempt < 2; attempt++)
                    {
                        if (randomness == 0 && canGoLeft && leftIndex > 0)
                        {
                            // check if left block is empty
                            if (left->type == VOID)
                            {
                                left->type = WATER;
                                left->color = BLUE; 
                                left->exists = true;
                                left->rect.x = ( (i - 1) % GRID_WIDTH) * SCALE;
                                left->rect.y = (i / GRID_WIDTH) * SCALE;
                                left->wasUpdated = true;

                                self->wasUpdated = true; 
                                self->type = VOID; 
                                self->color = BLACK;
                                self->exists = false;
                                break;
                            }
                        }
                        else if (randomness == 1 && canGoRight && rightIndex <= static_cast<int>(grid.size() - 1)) 
                        {
                            // check if right block is empty
                            if (right->type == VOID)
                            {
                                right->type = WATER;
                                right->color = BLUE;
                                right->exists = true;
                                right->rect.x = ( (i + 1) % GRID_WIDTH) * SCALE;
                                right->rect.y = (i / GRID_WIDTH) * SCALE;
                                right->wasUpdated = true; 

                                self->wasUpdated = true; 
                                self->type = VOID; 
                                self->color = BLACK;
                                self->exists = false;
                                break;
                            }
                        }
                    }
                }
            }
        }
        else if (self->type == FIRE)
        {

        }
        else if(self->type == IRON)
        {

        }
        else if(self->type == GAS)
        {
            int indexAbove = i - GRID_WIDTH;
            if (indexAbove < 0) { continue; }
            Particle* above = &grid.at(indexAbove);
 
            // if gas matches fire, it becomes a gas 
            if(above->type == FIRE)
            {
                self->type = FIRE;
                self->color = RED;
                self->exists = true;
                self->wasUpdated = true;
                continue;
            }

            if((above->type == VOID || above->type == WATER) && !above->wasUpdated)
            {
                TYPES previousType = above->type; 
                above->type = GAS;
                above->color = SKYBLUE;
                above->exists = true;
                above->wasUpdated = true;

                if(previousType == WATER)
                {
                    self->type = WATER;
                    self->color = BLUE;
                }
                else if(previousType == SAND)
                {
                    self->type = SAND;
                    self->color = YELLOW;
                }
                else // void 
                {
                    self->type = VOID;
                    self->color = BLACK;
                    self->exists = false; 
                }
                self->wasUpdated = true;
                continue;
            }

            bool canGoLeft = (i % GRID_WIDTH > 0);
            bool canGoRight = (i % GRID_WIDTH <  GRID_WIDTH - 1);
            int leftIndex = i - 1;
            int rightIndex = i + 1;
            int randomness = GetRandomValue(0, 1);
            for(int attempts = 0; attempts < 2; attempts++)
            {
                // left and right
                if(randomness == 1 && canGoLeft && leftIndex > 0)
                {
                    Particle* left = &grid.at(leftIndex);
                    if (left->type == VOID)
                    {
                        // update new,  resets old 
                        left->type = GAS;
                        left->color = SKYBLUE;
                        left->exists = true;
                        left->wasUpdated = true; 

                        self->type = VOID;
                        self->color = BLACK;
                        self->exists = false;
                        self->wasUpdated = true;
                        break;
                    }
                }
                else if(randomness == 0 && canGoRight && rightIndex < static_cast<int>(grid.size()) - 1)
                {
                    Particle* right = &grid.at(rightIndex);
                    if (right->type == VOID)
                    {
                        // update new, resets old 
                        right->type = GAS;
                        right->color = SKYBLUE;
                        right->exists = true;
                        right->wasUpdated = true;

                        self->type = VOID;
                        self->color = BLACK;
                        self->exists = false;
                        self->wasUpdated = true; 
                        break;
                    }
                }
            }
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
 
            saveParticles(grid, selectedElement);
            if (simulationTimer >= maxTime)
            {
                calculatePhysics(grid);
                simulationTimer = 0.0f;
            }

            drawParticles(grid);
            DrawFPS(WIDTH - 200, 30);
            DrawText("Sandbox simulator", WIDTH - 200, 10, 21, GREEN);
            selectedElement = checkSelectedElement(selectedElement); 

        EndDrawing();
    }
    CloseWindow();
    return 0;
}

