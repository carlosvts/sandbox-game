/*
 * This file contains all necessary headers for utilizing a bitmap (.bmp)
 * Source:
 * https://solarianprogrammer.com/2018/11/19/cpp-reading-writing-bmp-images/ We
 * usually divide bmp in 4 parts file header, bitmap header, color header and
 * pixel data
 */
// This header was imported from my project:
// https://github.com/carlosvts/raw-image-processor
#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

constexpr uint16_t BITMAP_FILETYPE = 0x4D42;

#pragma pack(push, 1) // ensures that no padding happens (BMP needs a multiple
                      // of WORD to work)
struct BMPFileHeader
{
    uint16_t fileType{BITMAP_FILETYPE};
    uint32_t fileSize{0};
    uint16_t reserved1{0};
    uint16_t reserved2{0};
    uint32_t offsetData{
        0}; // pos in bytes from start of file where pixel starts
};

struct BMPInfoHeader
{
    uint32_t size{0};
    int32_t width{0};
    int32_t height{0};

    uint16_t planes{1};      // num of planes for the target device, always 1
    uint16_t bitCount{0};    // bits per pixel
    uint32_t compression{0}; // 0 or 3, umcompressed or not
    uint32_t sizeImage{0};   // 0 fur umcompressed images
    int32_t xPixelsPerMeter{0};
    int32_t yPixelsPerMeter{0};
    uint32_t colors_used{0}; // Num  color indexes in the color table. Use 0 for
                             // the max number of colors allowed by bit_count
    uint32_t colors_important{0}; // Num of colors used for displaying the
                                  // bitmap. If 0 all colors are required
};

// BMP uses BGRA and LITTLE-ENDIAN pattern
struct BMPColorHeader
{
    uint32_t redMask{0x00ff0000};          // red channel
    uint32_t greenMask{0x0000ff00};        // green channel
    uint32_t blueMask{0x000000ff};         // blue channel
    uint32_t alphaMask{0xff000000};        // alpha channel
    uint32_t color_space_type{0x73524742}; // Default "sRGB" (0x73524742)
    uint32_t unused[16]{0};                // Unused data for sRGB color space
};

#pragma pack(pop) // resets configuration for padding

struct BMP
{
    BMPFileHeader fileHeader;
    BMPInfoHeader infoHeader;
    BMPColorHeader colorHeader;
    std::vector<uint8_t> data;
    int bitsInBGR = 24;
    int bitsInBGRA = 32;

    // Constructors
    BMP(const char *bmpFile)
    {
        read(bmpFile);
    }

    // Copy Constructor
    BMP(const BMP &other)
    {
        fileHeader = other.fileHeader;
        infoHeader = other.infoHeader;
        colorHeader = other.colorHeader;
        data = other.data;
    }

    // To match rule of Three
    BMP &operator=(const BMP &other)
    {
        if (this != &other)
        {
            fileHeader = other.fileHeader;
            infoHeader = other.infoHeader;
            colorHeader = other.colorHeader;
            data = other.data;
        }
        return *this;
    }

    ~BMP() = default;

    // We will read a bmp, create another instance, modify it and write in
    // another file in this project.
    void read(const char *bmpFile)
    {
        std::ifstream istream(bmpFile, std::ios::binary);
        if (!istream.is_open())
        {
            throw std::runtime_error("Unable to open this file");
        }
        else
        {
            istream.read(reinterpret_cast<char *>(&fileHeader),
                         sizeof(fileHeader));
            if (fileHeader.fileType != BITMAP_FILETYPE)
            {
                throw std::runtime_error(
                    "Unable to read this file: not in a recognizable format.");
            }
            // now cursor is in infoHeader
            istream.read(reinterpret_cast<char *>(&infoHeader),
                         sizeof(infoHeader));
            // jump ofset data
            istream.seekg(fileHeader.offsetData, std::ios::beg);

            int channels = infoHeader.bitCount / 8;

            if (infoHeader.bitCount == bitsInBGR)
            {
                data.resize(infoHeader.width * infoHeader.height * channels);

                std::streamsize padding =
                    ((4 - (infoHeader.width * channels) % 4) % 4);

                istream.seekg(fileHeader.offsetData, std::ios::beg);

                for (int i = 0; i < infoHeader.height; ++i)
                {
                    istream.read(reinterpret_cast<char *>(
                                     &data[i * infoHeader.width * channels]),
                                 infoHeader.width * channels);

                    istream.seekg(padding, std::ios::cur);
                }
            }
            else if (infoHeader.bitCount == bitsInBGRA)
            {
                data.resize(infoHeader.width * infoHeader.height * channels);

                istream.seekg(fileHeader.offsetData, std::ios::beg);

                for (int i = 0; i < infoHeader.height; ++i)
                {
                    istream.read(reinterpret_cast<char *>(
                                     &data[i * infoHeader.width * channels]),
                                 infoHeader.width * channels);
                }
            }
        }
    }
    // Creates a new bitmap file and write data into it
    void write(const char *bmpFile)
    {
        std::ofstream ostream(bmpFile, std::ios::binary | std::ios::out |
                                           std::ios::trunc);
        if (!ostream.is_open())
        {
            throw std::runtime_error("Error while creating output file");
        }
        else
        {
            // writing file header
            ostream.write(reinterpret_cast<char *>(&fileHeader),
                          sizeof(fileHeader));
            // writing info header
            ostream.write(reinterpret_cast<char *>(&infoHeader),
                          sizeof(infoHeader));
            // jumping offset
            ostream.seekp(fileHeader.offsetData, std::ios::beg);

            int channels = infoHeader.bitCount / 8;
            int rowSize = infoHeader.width * channels;
            std::streamsize padding = ((4 - (rowSize) % 4) % 4);

            for (int i = 0; i < infoHeader.height; ++i)
            {
                ostream.write(reinterpret_cast<char *>(&data[i * rowSize]),
                              rowSize);

                uint32_t zero = 0;
                ostream.write(reinterpret_cast<char *>(&zero), padding);
            }
            std::cout << "New image generated! " << std::endl;
        }
    }

    // for threshold filter
    // calculates a treshhold of a channel
    uint8_t calculateThreshold(uint8_t gray)
    {
        const int limit = 127;
        return gray >= limit ? 255 : 0;
    }

    // filters

    void toGrayScale()
    {
        int channels = infoHeader.bitCount / 8;

        // BGRA
        for (size_t i = 0; i < data.size(); i += channels)
        {
            uint8_t blue = data[i];
            uint8_t green = data[i + 1];
            uint8_t red = data[i + 2];

            // via CIE 1931 color space
            uint8_t gray = static_cast<uint8_t>(0.299 * red + 0.587 * green +
                                                0.114 * blue);
            data[i] = gray;
            data[i + 1] = gray;
            data[i + 2] = gray;
        }
    }

    void toSepia()
    {
        int channels = infoHeader.bitCount / 8;

        // BGRA
        for (size_t i = 0; i < data.size(); i += channels)
        {
            uint8_t blue = data[i];
            uint8_t green = data[i + 1];
            uint8_t red = data[i + 2];

            uint8_t newBlue =
                std::clamp(static_cast<int>((red * 0.272) + (green * 0.534) +
                                            (blue * 0.131)),
                           0, 255);
            uint8_t newGreen =
                std::clamp(static_cast<int>((red * 0.394) + (green * 0.686) +
                                            (blue * 0.168)),
                           0, 255);
            uint8_t newRed =
                std::clamp(static_cast<int>((red * 0.393) + (green * 0.769) +
                                            (blue * 0.189)),
                           0, 255);

            data[i] = newBlue;
            data[i + 1] = newGreen;
            data[i + 2] = newRed;
        }
    }

    void toNegative()
    {
        int channels = infoHeader.bitCount / 8;

        for (size_t i = 0; i < data.size(); i += channels)
        {
            data[i] = 255 - data[i];
            data[i + 1] = 255 - data[i + 1];
            data[i + 2] = 255 - data[i + 2];
        }
    }

    void toThreshold()
    {
        int channels = infoHeader.bitCount / 8;

        for (size_t i = 0; i < data.size(); i += channels)
        {
            uint8_t blue = data[i];
            uint8_t green = data[i + 1];
            uint8_t red = data[i + 2];

            // via CIE 1931 color space
            uint8_t gray = static_cast<uint8_t>(0.299 * red + 0.587 * green +
                                                0.114 * blue);

            data[i] = calculateThreshold(gray);
            data[i + 1] = calculateThreshold(gray);
            data[i + 2] = calculateThreshold(gray);
        }
    }

    void applyGausianBlur()
    {
        int channels = infoHeader.bitCount / 8;
        std::vector<uint8_t> tmp = data;
        // kernel [ky][kx]
        float kernel[3][3] = {{1 / 16.0f, 2 / 16.0f, 1 / 16.0f},
                              {2 / 16.0f, 4 / 16.0f, 2 / 16.0f},
                              {1 / 16.0f, 2 / 16.0f, 1 / 16.0f}};

        // loop through image
        for (int i = 1; i < infoHeader.height - 1; ++i)
        {
            for (int j = 1; j < infoHeader.width - 1; ++j)
            {
                // loop in each channel
                for (int c = 0; c < 3; ++c)
                {
                    float accumulator = 0.0f;

                    // applying convolution kernel
                    for (int ky = -1; ky <= 1; ++ky)
                    {

                        for (int kx = -1; kx <= 1; ++kx)
                        {
                            int neighborIndex =
                                ((i + ky) * infoHeader.width + (j + kx)) *
                                    channels +
                                c;
                            float weight = kernel[ky + 1][kx + 1];
                            accumulator += data[neighborIndex] * weight;
                        }
                    }

                    int currentPixelIndex =
                        (i * infoHeader.width + j) * channels + c;
                    tmp[currentPixelIndex] = static_cast<uint8_t>(accumulator);
                }
            }
        }
        data = tmp;
    }

    void sobelEdgeDetector()
    {
        int channels = infoHeader.bitCount / 8;
        std::vector<uint8_t> tmp = data;

        // Grayscale
        this->toGrayScale();

        // Source: https://en.wikipedia.org/wiki/Sobel_operator
        float Gx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
        float Gy[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

        // image loop
        for (int i = 1; i < infoHeader.height - 1; ++i)
        {
            for (int j = 1; j < infoHeader.width - 1; ++j)
            {
                float accumulatorX = 0;
                float accumulatorY = 0;

                // kernel convolution
                for (int ky = -1; ky <= 1; ky++)
                {
                    for (int kx = -1; kx <= 1; kx++)
                    {
                        int neighborIndex =
                            ((i + ky) * infoHeader.width + (j + kx)) * channels;
                        accumulatorX +=
                            data.at(neighborIndex) * Gx[ky + 1][kx + 1];
                        accumulatorY +=
                            data.at(neighborIndex) * Gy[ky + 1][kx + 1];
                    }
                }
                int currentPixelIndex = (i * infoHeader.width + j) * channels;
                int computedGradient = std::sqrt(std::pow(accumulatorX, 2) +
                                                 std::pow(accumulatorY, 2));
                uint8_t edge =
                    static_cast<uint8_t>(std::clamp(computedGradient, 0, 255));
                tmp[currentPixelIndex] = edge;
                tmp[currentPixelIndex + 1] = edge;
                tmp[currentPixelIndex + 2] = edge;
            }
        }
        data = tmp;
    }
};
