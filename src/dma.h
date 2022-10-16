#pragma once

#include "common.h"

struct Gameboy;

struct DMA
{
    void start(uint8_t byte);
    void tick(uint64_t cycles, Gameboy& gb);

    uint8_t start_byte = 0;
    uint8_t current_byte = 0;
    uint8_t delay = 0;
    bool active = false;
};
