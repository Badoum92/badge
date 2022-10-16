#pragma once

#include <cstdint>

namespace bag
{
struct Image
{
    static Image create(const char* path);
    static Image create(const uint8_t* data, uint32_t width, uint32_t height);
    static void destroy(Image& image);

    void from_file(const char* path);
    void from_buffer(const uint8_t* data, uint32_t _width, uint32_t _height);
    void update(const uint8_t* data);
    void* to_ptr();

    uint32_t handle = 0;
    uint32_t width = 0;
    uint32_t height = 0;
};
} // namespace bag
