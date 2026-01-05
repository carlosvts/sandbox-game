#include "game.hpp"

#define RAYGUI_IMPLEMENTATION
#include <raygui.h> // rayguy is on my usr/local/include 

int checkSelectedElement(int &currentElement)
{
    GuiPanel({GUIPANEL_X, GUIPANEL_Y, GUIPANEL_WIDTH, GUIPANEL_HEIGHT}, "Particles");
    if (GuiButton({BUTTONPANEL_X, BUTTONPANEL_Y, BUTTON_WIDTH, BUTTON_HEIGHT}, "SAND")) { return SAND; }
    if (GuiButton({BUTTONPANEL_X, BUTTONPANEL_Y + 35, BUTTON_WIDTH, BUTTON_HEIGHT},"WATER")) { return WATER; }
    if (GuiButton({BUTTONPANEL_X, BUTTONPANEL_Y + 35 * 2, BUTTON_WIDTH, BUTTON_HEIGHT}, "FIRE")) { return FIRE; }
    if (GuiButton({BUTTONPANEL_X, BUTTONPANEL_Y + 35 * 3, BUTTON_WIDTH, BUTTON_HEIGHT}, "IRON")) { return IRON; }
    if (GuiButton({BUTTONPANEL_X, BUTTONPANEL_Y + 35 * 4, BUTTON_WIDTH, BUTTON_HEIGHT}, "GAS")) { return GAS; }
    if (GuiButton({BUTTONPANEL_X, BUTTONPANEL_Y + 35 * 5, BUTTON_WIDTH, BUTTON_HEIGHT}, "LAVA")) { return LAVA; }
    if (GuiButton({BUTTONPANEL_X, BUTTONPANEL_Y + 35 * 6, BUTTON_WIDTH, BUTTON_HEIGHT}, "CONWAY")) { return CONWAY; }
    if (GuiButton({BUTTONPANEL_X, BUTTONPANEL_Y   + 35 * 7, BUTTON_WIDTH, BUTTON_HEIGHT}, "VOID")) { return VOID; }
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
            case LAVA:
            {
                particle.type = LAVA;
                particle.color = ORANGE;
                break;
            }
            case CONWAY:
            {
                particle.type = CONWAY;
                particle.color = LIME;
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
                // if down is VOID or CONWAY
                if (other->type == VOID || other->type == CONWAY)
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
            // checks all moore neighbor
            for (int y = -1; y <= 1; ++y)
            {
                for(int x = -1; x <= 1; ++x)
                {
                    // ignores itself 
                    if (x == 0 && y == 0) { continue; }
                    int neighborX = (i % GRID_WIDTH) + x;
                    int neighborY = (i / GRID_WIDTH) + y;

                    if (neighborX >= 0 && neighborY < GRID_WIDTH && neighborY >= 0 && neighborY < static_cast<int>(grid.size() - 1) / GRID_WIDTH)
                    {
                        int index = neighborY * GRID_WIDTH + neighborX;
                        Particle *neighbor = &grid.at(index);

                        // check reactions with fire
                        if (neighbor->type == WATER)
                        {
                            neighbor->type = GAS;
                            neighbor->color = SKYBLUE;
                            neighbor->wasUpdated = true;
                        }
                        else if(neighbor->type == VOID)
                        {
                            if (GetRandomValue(0, 100) <= 10)
                            {
                                self->type = VOID;
                                self->color = BLACK;
                                self->wasUpdated = true;
                                self->exists = false;
                            }
                        }
                        else if(neighbor->type == SAND)
                        {
                            neighbor->type = LAVA;
                            neighbor->color = ORANGE;
                            neighbor->wasUpdated = true;
                        }
                        else if(neighbor->type == IRON)
                        {
                            neighbor->type = FIRE;
                            neighbor->color = RED;
                            neighbor->wasUpdated = true;
                        }
                        else if(neighbor->type == GAS || neighbor->type == CONWAY)
                        {
                            // if hits a gas or conway, convolutes all Moore neighbor to fire
                            for(int ky = -1; ky <= 1; ++ky)
                            {
                                for(int kx = -1; kx <= 1; ++kx)
                                {
                                    if(kx == 0 && ky == 0) { continue; }
                                    if(GetRandomValue(0, 100) < 15) { continue; } // just a randomness to more

                                    int k_neighborX = (i % GRID_WIDTH) + kx;
                                    int k_neighborY = (i / GRID_WIDTH) + ky;
                                    if (k_neighborX >= 0 && k_neighborY < GRID_WIDTH && k_neighborY >= 0 && k_neighborY < static_cast<int>(grid.size() / GRID_WIDTH))
                                    {
                                        int k_index = k_neighborY * GRID_WIDTH + k_neighborX;
                                        Particle* k_particle = &grid.at(k_index);
                                        k_particle->type = FIRE;
                                        k_particle->color = RED;
                                        k_particle->exists = true;
                                        k_particle->wasUpdated = true;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        else if(self->type == IRON)
        {
            continue; // iron is just a static block
        }
        else if(self->type == GAS)
        {
            int indexAbove = i - GRID_WIDTH;
            if (indexAbove < 0) { continue; }
            Particle* above = &grid.at(indexAbove);
 
            // if gas matches fire, it becomes a gas 
            if(above->type == FIRE) { continue; }

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
        else if (self->type == LAVA)
        {
            int index = i + GRID_WIDTH;
            // left, down, right 
            int leftIndex = i - 1;
            int rightIndex = i + 1;
            if (leftIndex <= 0 || rightIndex >= static_cast<int>(grid.size()) || i <= 0 || i >= static_cast<int>(grid.size())) { continue; }
            // std::vector<Particle*> others = {&grid.at(leftIndex), &grid.at(index), &grid.at(rightIndex)};
           
            if (index <static_cast<int>(grid.size()))
            {
                // randomness to emalute a viscosity effect
                if (GetRandomValue(0, 100) <= 80) { continue; } // 20% chance of moving
                Particle* under = &grid.at(index);
            
                // if the under is void, just go down again
                if ((under->type == VOID || !under->exists || under->type == WATER || under->type == IRON  || under->type == GAS || under->type == FIRE) && !under->wasUpdated)
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
                else if (under->type == LAVA)
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
                                left->type = LAVA;
                                left->color = ORANGE; 
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
                                right->type = LAVA;
                                right->color = ORANGE;
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
                    // adds extra sauce with conway in a really unique circunstance
                    for (int y = -1; y <= 1; y++) 
                    {
                        for (int x = -1; x <= 1; x++) 
                        {
                            if (x == 0 && y == 0) continue; // Ignores itself 

                            int checkX = (i % GRID_WIDTH) + x;
                            int checkY = (i / GRID_WIDTH) + y;

                            if (checkX >= 0 && checkX < GRID_WIDTH && checkY >= 0 && checkY < (int)(grid.size() / GRID_WIDTH)) 
                            {
                                int targetIdx = checkY * GRID_WIDTH + checkX;
                                Particle* neighbor = &grid[targetIdx];

                                // if lava encounters water, it can generate a conway particle 
                                if (neighbor->type == WATER) 
                                {
                                    // 20%  (CONWAY), 80% (GAS)
                                    if (GetRandomValue(0, 100) < 20) 
                                    {
                                        neighbor->type = CONWAY;
                                        neighbor->color = LIME;
                                        neighbor->exists = true;
                                    } 
                                    else 
                                    {
                                        neighbor->type = GAS;
                                        neighbor->color = SKYBLUE;
                                        neighbor->exists = true;
                                    }
                                    neighbor->wasUpdated = true;
                                    self->type = VOID;
                                }
                            }
                        }
                    }
                }
            }
        }
        else if (self->type == CONWAY)
        {
            int neighbors {0};
            for (int y = -1; y <= 1; y++) 
            {
                for (int x = -1; x <= 1; x++) 
                {
                    if (x == 0 && y == 0) continue;

                    int checkX = (i % GRID_WIDTH) + x;
                    int checkY = (i / GRID_WIDTH) + y;

                    if (checkX >= 0 && checkX < GRID_WIDTH && checkY >= 0 && checkY < (int)(grid.size() / GRID_WIDTH)) 
                    {
                        int targetIdx = checkY * GRID_WIDTH + checkX; 
                        if (grid.at(targetIdx).exists) { neighbors++; }
                    }
                }
            }
            // EDITED VERSION, now particles can survive if they are alone
            if (neighbors > CONWAY_SURVIVAL_THRESHOLD)
            {
                self->type = VOID;
                self->color = BLACK;
                self->exists = false;
                self->wasUpdated = true;
            }
            else
            {
                self->wasUpdated = true;
                if (GetRandomValue(0, 100) <= 30) // 30% chance of infecting other particle 
                {
                   int offsets[] = { -GRID_WIDTH, GRID_WIDTH, -1, 1 };
                    for (int x = 0; x < 4; x++)
                    {
                        int targetIdx = i + offsets[x];

                        if (targetIdx >= 0 && targetIdx < static_cast<int>(grid.size()))
                        {
                            bool isSide = (offsets[x] == -1 || offsets[x] == 1);
                            if (isSide)
                            {
                                if ((offsets[x] == -1 && i % GRID_WIDTH == 0) || (offsets[x] == 1 && i % GRID_WIDTH == GRID_WIDTH - 1)) { continue; }
                            }

                            Particle* target = &grid.at(targetIdx);
                            if (!target->exists && GetRandomValue(0, 100) < 5)
                            {
                                target->type = CONWAY;
                                target->color = LIME;
                                target->exists = true;
                                target->wasUpdated = true;
                            }
                        }
                    }
                }
            }
        }
    }
}
