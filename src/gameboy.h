#pragma once

#include "memory.h"
#include "cpu.h"
#include "instruction.h"

struct Gameboy
{
    Gameboy();

    void reset();
    bool load_rom(const char* path);

    void step();
    void execute_instruction(const Instruction& instruction);
    Instruction fetch_instruction();
    void fetch_data(Instruction& instruction);

    Memory memory;
    CPU cpu;
};
