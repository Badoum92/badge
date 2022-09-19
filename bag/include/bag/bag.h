#pragma once

#include <string>

#include <imgui/imgui.h>

#include "image.h"

namespace bag
{
struct Options
{
    uint32_t width = 1280;
    uint32_t height = 720;
    std::string title = "Bag window";
    bool resizeable = false;
    bool vsync = true;
};

class Application
{
public:
    Application(const Options& options, bool full_window_imgui = false);
    ~Application();
    Application(const Application&) = delete;

    void run();
    virtual void update(const Options& options) = 0;

private:
    void render();

    Options options_;
    bool full_window_imgui_ = false;
};
} // namespace bag
