#pragma once

#include "memory.h"
#include "cpu.h"
#include "ppu.h"
#include "instruction.h"
#include "dma.h"

struct CartInfo
{
    char title[17] = {};
    uint8_t cgb = 0;
    uint8_t sgb = 0;
    uint8_t cartridge_type = 0;
    uint8_t rom_size = 0;
    uint8_t ram_size = 0;
    uint8_t header_checksum = 0;
};

struct Gameboy
{
    Gameboy();

    void reset();
    bool load_rom(const char* path);

    void step();
    uint32_t execute_instruction(const Instr& instr);
    Instr fetch_instruction();
    void process_serial_data();

    CartInfo cart_info;
    Memory memory;
    CPU cpu;
    PPU ppu;
    DMA dma;

    bool stepping = true;

    std::string serial_data;
};

extern Gameboy gb;
