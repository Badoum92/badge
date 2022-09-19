#include "image.h"

#include <stdexcept>
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

namespace bag
{
Image Image::create(const char* path)
{
    Image image;
    image.from_file(path);
    return image;
}

Image Image::create(const uint8_t* data, uint32_t _width, uint32_t _height)
{
    Image image;
    image.from_buffer(data, _width, _height);
    return image;
}

void Image::destroy(Image& image)
{
    glDeleteTextures(1, &image.handle);
    image.handle = 0;
    image.width = 0;
    image.height = 0;
}

void Image::from_file(const char* path)
{
    int w = 0;
    int h = 0;
    uint8_t* data = stbi_load(path, &w, &h, nullptr, 4);
    if (data == nullptr)
    {
        return;
    }
    from_buffer(data, w, h);
    stbi_image_free(data);
}

void Image::from_buffer(const uint8_t* data, uint32_t _width, uint32_t _height)
{
    destroy(*this);
    width = _width;
    height = _height;

    glCreateTextures(GL_TEXTURE_2D, 1, &handle);
    glTextureParameteri(handle, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(handle, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureStorage2D(handle, 1, GL_RGBA8, width, height);
    glTextureSubImage2D(handle, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
}

void* Image::to_ptr()
{
    return (void*)(intptr_t)handle;
}
} // namespace bag
