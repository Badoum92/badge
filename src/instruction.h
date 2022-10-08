#pragma once

#include <string>

#include "common.h"
#include "enum_array.h"

struct Gameboy;

enum class Reg
{
    NONE,
    A,
    F,
    B,
    C,
    D,
    E,
    H,
    L,
    AF,
    BC,
    DE,
    HL,
    SP,
    PC,
    Count
};

static constexpr EnumArray<Reg, const char*> reg_str = {"NONE", "A",  "F",  "B",  "C",  "D",  "E", "H",
                                                        "L",    "AF", "BC", "DE", "HL", "SP", "PC"};

enum class Cond
{
    NONE,
    NZ,
    Z,
    NC,
    C,
    Count
};

static constexpr EnumArray<Cond, const char*> cond_str = {"NONE", "NZ", "Z", "NC", "C"};

struct Instr
{
    using Exec = uint32_t (*)(Gameboy&, const Instr&);
    using ToString = std::string (*)(Gameboy&, const Instr&);

    uint8_t opcode = 0;
    Exec exec = nullptr;
    ToString to_string = nullptr;
    Reg r1 = Reg::NONE;
    Reg r2 = Reg::NONE;
    Cond cond = Cond::NONE;
};

extern Instr instructions[0x100];
