#ifndef GAME_H
#define GAME_H

#include "utils.hpp"
#include <vector>
// 900 x 600 default
constexpr const int WIDTH = 1200;
constexpr const int HEIGHT = 1200;

constexpr const int SCALE = 6;
constexpr const int GRID_WIDTH = WIDTH / SCALE;
constexpr const int GRID_HEIGHT = HEIGHT / SCALE;
constexpr const int FPS = 120;

constexpr const int GUIPANEL_X = 10;
constexpr const int GUIPANEL_Y = 10;
constexpr const int GUIPANEL_WIDTH = 120;
constexpr const int GUIPANEL_HEIGHT = 320;

constexpr const int BUTTONPANEL_X = 20;
constexpr const int BUTTONPANEL_Y = GUIPANEL_Y + 35;
constexpr const int BUTTON_WIDTH = 100;
constexpr const int BUTTON_HEIGHT = 30;

constexpr const int CONWAY_SURVIVAL_THRESHOLD = 5;

int checkSelectedElement(int &currentElement);
void saveParticles(std::vector<Particle> &grid, int selectedElement);
void drawParticles(std::vector<Particle> &grid);
void calculatePhysics(std::vector<Particle> &grid);
#endif
