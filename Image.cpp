#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image.h"
#include "stb_image_write.h"

#include "Image.h"

#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

Image::Image(const char *filename, int channel_force)
{
    if (read(filename, channel_force))
    {
        printf("Read %s\n", filename);
        size = w * h * channels;
    }
    else
    {
        printf("Failed to read %s\n", filename);
    }
}

Image::Image(int w, int h, int channels) : w(w), h(h), channels(channels)
{
    size = w * h * channels;
    data = new uint8_t[size];
}

Image::Image(const Image &img) : Image(img.w, img.h, img.channels)
{
    memcpy(data, img.data, size);
}

Image::~Image()
{
    stbi_image_free(data);
}

bool Image::read(const char *filename, int channel_force)
{
    data = stbi_load(filename, &w, &h, &channels, channel_force);
    channels = channel_force == 0 ? channels : channel_force;
    return data != NULL;
}

bool Image::write(const char *filename)
{
    ImageType type = get_file_type(filename);
    int success;
    switch (type)
    {
    case PNG:
        success = stbi_write_png(filename, w, h, channels, data, w * channels);
        break;
    case BMP:
        success = stbi_write_bmp(filename, w, h, channels, data);
        break;
    case JPG:
        success = stbi_write_jpg(filename, w, h, channels, data, 100);
        break;
    case TGA:
        success = stbi_write_tga(filename, w, h, channels, data);
        break;
    }
    if (success != 0)
    {
        printf("\e[32mWrote \e[36m%s\e[0m, %d, %d, %d, %zu\n", filename, w, h, channels, size);
        return true;
    }
    else
    {
        printf("\e[31;1m Failed to write \e[36m%s\e[0m, %d, %d, %d, %zu\n", filename, w, h, channels, size);
        return false;
    }
}

ImageType Image::get_file_type(const char *filename)
{
    const char *ext = strrchr(filename, '.');
    if (ext != nullptr)
    {
        if (strcmp(ext, ".png") == 0)
        {
            return PNG;
        }
        else if (strcmp(ext, ".jpg") == 0)
        {
            return JPG;
        }
        else if (strcmp(ext, ".bmp") == 0)
        {
            return BMP;
        }
        else if (strcmp(ext, ".tga") == 0)
        {
            return TGA;
        }
    }
    return PNG;
}

Image &Image::grayscale_avg()
{
    if (channels < 3)
    {
        printf("Image %p has less than 3 channels, it is assumed to already be grayscale.", this);
    }
    else
    {
        auto startTime = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < size; i += channels)
        {
            // if (i < 100)
            //     printf("%d ", i);
            //(r+g+b)/3
            int gray = (data[i] + data[i + 1] + data[i + 2]) / 3;
            memset(data + i, gray, 3);
        }

        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
        std::cout << "Time elapsed: " << duration << " milliseconds" << std::endl;
    }
    return *this;
}

Image &Image::grayscale_lum()
{
    if (channels < 3)
    {
        printf("Image %p has less than 3 channels, it is assumed to already be grayscale.", this);
    }
    else
    {
        for (int i = 0; i < size; i += channels)
        {
            int gray = 0.3 * data[i] + 0.59 * data[i + 1] + 0.11 * data[i + 2];
            memset(data + i, gray, 3);
        }
    }
    return *this;
}

Image &Image::grayscale_avg_parallel()
{
    if (channels < 3)
    {
        printf("Image %p has less than 3 channels, it is assumed to already be grayscale.", this);
    }
    else
    {
        std::vector<std::thread> threads;
        int numThreads = 3;
        int chunkSize = size / numThreads;

        auto startTime = std::chrono::high_resolution_clock::now();
        for (int t = 0; t < numThreads; ++t)
        {
            threads.emplace_back([this, t, chunkSize, numThreads]()
                                 {
                    int start = t * chunkSize;
                    int end = (t == numThreads - 1) ? size : (start + chunkSize);
                    for (int i = start; i < end; i += channels) {
                        int gray = (data[i] + data[i + 1] + data[i + 2] ) / 3;
                        memset(data + i, gray, 3);
                    } });
        }

        for (auto &thread : threads)
        {
            thread.join();
        }

        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
        std::cout << "Time elapsed: " << duration << " milliseconds" << std::endl;
    }
    return *this;
}

Image &Image::color_mask(float r, float g, float b)
{
    if (channels < 3)
    {
        printf("\e[31m[ERROR] Color mask requires at least 3 channels, but this image has %d channels\e[0m\n", channels);
    }
    else
    {
        for (int i = 0; i < size; i += channels)
        {
            data[i] *= r;
            data[i + 1] *= g;
            data[i + 2] *= b;
        }
    }
    return *this;
}

Image &Image::to_green()
{
    if (channels < 3)
    {
        printf("\e[31m[ERROR] Color mask requires at least 3 channels, but this image has %d channels\e[0m\n", channels);
    }
    else
    {
        for (int i = 0; i < size; i += channels)
        {
            uint8_t temp = data[i + 1];
            data[i + 1] = data[i + 2];
            data[i + 2] = temp;
        }
    }
    return *this;
}