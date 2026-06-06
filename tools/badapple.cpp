/*
 * This file will process each image from frames_bmp
 * And make it into a binary readble file
 * So i can render badapple using my engine
 */
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include <raylib.h>

#include "bitmap.hpp"
#include "game.hpp"

constexpr float FRAME_TIME = 1.0f / 30.0f;

std::string framePath(int frame)
{
    std::ostringstream oss;

    oss << "frames_bmp/frame_" << std::setw(6) << std::setfill('0') << frame
        << ".bmp";

    return oss.str();
}

void loadFrameToGrid(const BMP &frame, std::vector<Particle> &grid)
{
    const int channels = frame.infoHeader.bitCount / 8;

    const int width = std::min(GRID_WIDTH, frame.infoHeader.width);

    const int height = std::min(GRID_HEIGHT, frame.infoHeader.height);

    for (Particle &particle : grid)
    {
        particle.type = VOID;
        particle.exists = false;
        particle.color = BLACK;
        particle.wasUpdated = false;
    }

    for (int y = 0; y < height; y++)
    {
        int flippedY = frame.infoHeader.height - 1 - y;

        for (int x = 0; x < width; x++)
        {
            int bmpIndex = (flippedY * frame.infoHeader.width + x) * channels;

            int gridIndex = y * GRID_WIDTH + x;

            uint8_t pixel = frame.data[bmpIndex];

            Particle &p = grid[gridIndex];

            if (pixel == 0)
            {
                p.type = SAND;
                p.exists = true;
                p.color = YELLOW;
                p.wasUpdated = false;

                p.rect = {static_cast<float>(x * SCALE),
                          static_cast<float>(y * SCALE), SCALE, SCALE};
            }
        }
    }
}

int main()
{
    std::vector<Particle> grid(GRID_WIDTH * GRID_HEIGHT);

    int currentFrame = 1;

    BMP frame(framePath(currentFrame).c_str());

    frame.toGrayScale();
    frame.toThreshold();

    loadFrameToGrid(frame, grid);

    InitWindow(WIDTH, HEIGHT, "Bad Apple Sandbox");

    SetTargetFPS(60);

    float accumulator = 0.0f;

    while (!WindowShouldClose())
    {
        accumulator += GetFrameTime();

        if (accumulator >= FRAME_TIME)
        {
            accumulator -= FRAME_TIME;

            currentFrame++;

            std::string path = framePath(currentFrame);

            if (std::filesystem::exists(path))
            {
                BMP next(path.c_str());

                next.toGrayScale();
                next.toThreshold();

                loadFrameToGrid(next, grid);
            }
            else
            {
                currentFrame = 1;
            }
        }

        BeginDrawing();

        ClearBackground(BLACK);

        drawParticles(grid);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
