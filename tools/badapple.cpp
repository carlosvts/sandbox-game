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

void clearBadAppleLayer(std::vector<Particle> &grid)
{
    for (Particle &p : grid)
    {
        if (p.type == IRON)
        {
            p.type = VOID;
            p.exists = false;
            p.color = BLACK;
            p.wasUpdated = false;
        }
    }
}

void injectFrame(const BMP &frame, std::vector<Particle> &grid)
{
    const int channels = frame.infoHeader.bitCount / 8;

    const int width = std::min(GRID_WIDTH, frame.infoHeader.width);

    const int height = std::min(GRID_HEIGHT, frame.infoHeader.height);

    clearBadAppleLayer(grid);

    for (int y = 0; y < height; y++)
    {
        int flippedY = frame.infoHeader.height - 1 - y;

        for (int x = 0; x < width; x++)
        {
            int bmpIndex = (flippedY * frame.infoHeader.width + x) * channels;

            int gridIndex = y * GRID_WIDTH + x;

            uint8_t pixel = frame.data[bmpIndex];

            if (pixel == 255)
            {
                Particle &p = grid[gridIndex];

                p.type = IRON;
                p.color = WHITE;
                p.exists = true;
                p.wasUpdated = false;

                p.rect = {static_cast<float>(x * SCALE),
                          static_cast<float>(y * SCALE), SCALE, SCALE};
            }
        }
    }
}

void spawnRandomParticles(std::vector<Particle> &grid)
{
    for (int i = 0; i < 5; ++i)
    {
        int x = GetRandomValue(0, GRID_WIDTH - 1);
        int y = GetRandomValue(0, 10);

        int idx = y * GRID_WIDTH + x;

        if (!grid[idx].exists)
        {
            Particle &p = grid[idx];

            p.type = SAND;
            p.color = YELLOW;
            p.exists = true;
            p.wasUpdated = false;

            p.rect = {static_cast<float>(x * SCALE),
                      static_cast<float>(y * SCALE), SCALE, SCALE};
        }
    }

    for (int i = 0; i < 5; ++i)
    {
        int x = GetRandomValue(0, GRID_WIDTH - 1);
        int y = GetRandomValue(0, 10);

        int idx = y * GRID_WIDTH + x;

        if (!grid[idx].exists)
        {
            Particle &p = grid[idx];

            p.type = WATER;
            p.color = BLUE;
            p.exists = true;
            p.wasUpdated = false;

            p.rect = {static_cast<float>(x * SCALE),
                      static_cast<float>(y * SCALE), SCALE, SCALE};
        }
    }
}

int main()
{
    std::vector<Particle> grid(GRID_WIDTH * GRID_HEIGHT);

    for (Particle &p : grid)
    {
        p.type = VOID;
        p.exists = false;
        p.color = BLACK;
        p.wasUpdated = false;
    }

    int currentFrame = 1;

    InitWindow(WIDTH, HEIGHT, "Bad Apple Sandbox");

    SetTargetFPS(60);

    float accumulator = 0.0f;

    while (!WindowShouldClose())
    {
        accumulator += GetFrameTime();

        if (accumulator >= FRAME_TIME)
        {
            accumulator -= FRAME_TIME;

            std::string path = framePath(currentFrame);

            if (std::filesystem::exists(path))
            {
                BMP frame(path.c_str());

                frame.toGrayScale();
                frame.toThreshold();

                injectFrame(frame, grid);

                currentFrame++;
            }
            else
            {
                currentFrame = 1;
            }
        }

        spawnRandomParticles(grid);

        calculatePhysics(grid);

        BeginDrawing();

        ClearBackground(BLACK);

        drawParticles(grid);

        DrawText(TextFormat("Frame: %d", currentFrame), 10, 10, 20, WHITE);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
