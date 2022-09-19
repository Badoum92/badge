#pragma once

#include <string>

struct GLFWwindow;

namespace bag
{
struct Options;

class Window
{
public:
    Window() = delete;

    static void create(Options& options);
    static void destroy();

    static uint32_t width();
    static uint32_t height();
    static void resized(uint32_t width, uint32_t height);

    static void poll_events();
    static void wait_events();
    static void swap_buffers();
    static bool should_close();
    static void close();
    static void set_title(const std::string& title);
    static void set_vsync(bool vsync);

    static GLFWwindow* handle();

private:
    inline static Options* options_ = nullptr;
    inline static GLFWwindow* handle_ = nullptr;
};
} // namespace bag
