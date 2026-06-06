/*
 * This file will process each image from frames_bmp
 * And make it into a binary readble file
 * So i can render badapple using my engine
 */
#include <iostream>
#include <raylib.h>

#include "bitmap.hpp"

constexpr int CELL_SIZE = 5;

int main()
{
    BMP frame("frames_bmp/frame_000001.bmp");
    std::cout << frame.infoHeader.width << '\n';
    std::cout << frame.infoHeader.height << '\n';
    std::cout << frame.infoHeader.bitCount << '\n';
    std::cout << frame.data.size() << '\n';
    frame.toGrayScale();
    frame.toThreshold();

    const int channels = frame.infoHeader.bitCount / 8;

    const int width = frame.infoHeader.width;
    const int height = frame.infoHeader.height;

    InitWindow(width * CELL_SIZE, height * CELL_SIZE, "Bad Apple Test");

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(BLACK);

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                int pixelIndex = (y * width + x) * channels;

                uint8_t pixel = frame.data[pixelIndex];

                DrawRectangle(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE,
                              CELL_SIZE, pixel ? WHITE : BLACK);
            }
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
