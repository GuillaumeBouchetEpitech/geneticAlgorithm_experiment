
#include "Data.hpp"

void Data::initialiseTextures()
{
    { // textures

        { // font

            const bool pixelated = true;
            const bool repeat = false;
            graphic.textures.textFont.load("assets/textures/ascii_font.png", pixelated, repeat);

        } // font

        { // chessboard

            const glm::ivec2 size = { 512, 512 };
            auto pixelsPtr = std::make_unique<unsigned char[]>(size.x * size.y * 4);
            unsigned char* rawPixels = pixelsPtr.get();

            const auto setPixel = [&size, rawPixels](
                int x, int y,
                unsigned char grey,
                unsigned char alpha)
            {
                rawPixels[y * 4 * size.x + x * 4 + 0] = grey;
                rawPixels[y * 4 * size.x + x * 4 + 1] = grey;
                rawPixels[y * 4 * size.x + x * 4 + 2] = grey;
                rawPixels[y * 4 * size.x + x * 4 + 3] = alpha;
            };

            for (int yy = 0; yy < size.y; ++yy)
            for (int xx = 0; xx < size.x; ++xx)
            {
                if ((xx < size.x * 0.5 && yy < size.y * 0.5) ||
                    (xx > size.x * 0.5 && yy > size.y * 0.5))
                {
                    setPixel(xx, yy, 32, 220);
                }
                else
                {
                    setPixel(xx, yy, 100, 255);
                }
            }

            bool pixelated = false;
            bool repeat = true;
            graphic.textures.chessboard.allocateBlank(size, pixelated, repeat, rawPixels);

        } // chessboard

        { // cylinders

            auto fillBuffer = [](const glm::ivec2& size, unsigned char* rawPixels, int minX, int maxX)
            {
                for (int yy = 0; yy < size.y; ++yy)
                for (int xx = 0; xx < size.x; ++xx)
                {
                    if (xx >= minX && xx < maxX)
                    {
                        if (yy < size.y * 0.25f)
                        {
                            // blue
                            rawPixels[yy * 4 * size.x + xx * 4 + 0] = 0;
                            rawPixels[yy * 4 * size.x + xx * 4 + 1] = 0;
                            rawPixels[yy * 4 * size.x + xx * 4 + 2] = 128;
                        }
                        else
                        if (yy < size.y * 0.5f)
                        {
                            // gray
                            rawPixels[yy * 4 * size.x + xx * 4 + 0] = 128;
                            rawPixels[yy * 4 * size.x + xx * 4 + 1] = 128;
                            rawPixels[yy * 4 * size.x + xx * 4 + 2] = 128;
                        }
                        else
                        if (yy < size.y * 0.75f)
                        {
                            // red
                            rawPixels[yy * 4 * size.x + xx * 4 + 0] = 128;
                            rawPixels[yy * 4 * size.x + xx * 4 + 1] = 0;
                            rawPixels[yy * 4 * size.x + xx * 4 + 2] = 0;
                        }
                        else
                        {
                            // green
                            rawPixels[yy * 4 * size.x + xx * 4 + 0] = 0;
                            rawPixels[yy * 4 * size.x + xx * 4 + 1] = 128;
                            rawPixels[yy * 4 * size.x + xx * 4 + 2] = 0;
                        }
                        rawPixels[yy * 4 * size.x + xx * 4 + 3] = 255;
                    }
                    else
                    {
                        // transparent
                        rawPixels[yy * 4 * size.x + xx * 4 + 0] = 0;
                        rawPixels[yy * 4 * size.x + xx * 4 + 1] = 0;
                        rawPixels[yy * 4 * size.x + xx * 4 + 2] = 0;
                        rawPixels[yy * 4 * size.x + xx * 4 + 3] = 0;
                    }
                }
            };

            constexpr bool pixelated = true;
            constexpr bool repeat = true;

            const glm::ivec2 size = { 512, 512 };
            auto pixelsPtr = std::make_unique<unsigned char[]>(size.x * size.y * 4);
            unsigned char* rawPixels = pixelsPtr.get();

            fillBuffer(size, rawPixels, size.x * 0.00f, size.x * 0.25f);
            graphic.textures.cylinders[0].allocateBlank(size, pixelated, repeat, rawPixels);

            fillBuffer(size, rawPixels, size.x * 0.25f, size.x * 0.50f);
            graphic.textures.cylinders[1].allocateBlank(size, pixelated, repeat, rawPixels);

            fillBuffer(size, rawPixels, size.x * 0.50f, size.x * 0.75f);
            graphic.textures.cylinders[2].allocateBlank(size, pixelated, repeat, rawPixels);

            fillBuffer(size, rawPixels, size.x * 0.75f, size.x * 1.00f);
            graphic.textures.cylinders[3].allocateBlank(size, pixelated, repeat, rawPixels);

        } // cylinders

    } // textures
}
