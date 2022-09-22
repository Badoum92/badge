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

void init(const Options& options, bool full_window_imgui);
void terminate();
void begin_frame();
void end_frame();
bool running();
const Options& options();
} // namespace bag
