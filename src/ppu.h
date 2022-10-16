#pragma once

#include "common.h"

struct Gameboy;

struct OAMEntry
{
    uint8_t y_pos;
    uint8_t x_pos;
    uint8_t tile_index;
    uint8_t cgb_palette_number : 3;
    uint8_t tile_vram_bank : 1;
    uint8_t palette_number : 1;
    uint8_t x_flip : 1;
    uint8_t y_flip : 1;
    uint8_t bg_prio : 1;
};

struct PPU
{
    constexpr static uint32_t colors[] = {0xffffffff, 0xffaaaaaa, 0xff555555, 0xff000000};

    void reset(Gameboy& gb);

    OAMEntry* OAM_table;
    uint8_t* vram;
};
