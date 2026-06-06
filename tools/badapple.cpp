/*
 * This file will process each image from frames_bmp
 * And make it into a binary readble file
 * So i can render badapple using my engine
 */

#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>

#include <raylib.h>

#include "bitmap.hpp"

constexpr int CELL_SIZE = 5;
constexpr float FRAME_TIME = 1.0f / 30.0f;

std::string framePath(int frame)
{
    // building path to get every file
    std::ostringstream oss;

    oss << "frames_bmp/frame_" << std::setw(6) << std::setfill('0') << frame
        << ".bmp";

    return oss.str();
}

int main()
{
    int currentFrame = 1;

    BMP bmp(framePath(currentFrame).c_str());

    int width = bmp.infoHeader.width;
    int height = bmp.infoHeader.height;
    int channels = bmp.infoHeader.bitCount / 8;

    InitWindow(width * CELL_SIZE, height * CELL_SIZE, "Bad Apple");

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
                bmp = BMP(path.c_str());
            }
            else
            {
                currentFrame = 1;
                bmp = BMP(framePath(currentFrame).c_str());
            }
        }

        BeginDrawing();

        ClearBackground(BLACK);

        for (int y = 0; y < height; y++)
        {
            int flippedY = height - 1 - y;

            for (int x = 0; x < width; x++)
            {
                int index = (flippedY * width + x) * channels;

                uint8_t b = bmp.data[index + 0];
                uint8_t g = bmp.data[index + 1];
                uint8_t r = bmp.data[index + 2];

                uint8_t gray =
                    static_cast<uint8_t>(0.299f * r + 0.587f * g + 0.114f * b);

                uint8_t pixel = gray >= 127 ? 255 : 0;

                DrawRectangle(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE,
                              CELL_SIZE, pixel ? WHITE : BLACK);
            }
        }

        DrawText(TextFormat("Frame: %d", currentFrame), 10, 10, 20, RED);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
