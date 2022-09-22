#pragma once

#include <string>

#include "common.h"
#include "enum_array.h"

struct Gameboy;

enum class Operand
{
    NONE,
    R,
    MR,
    MRI,
    MRD,
    MRIO,
    D8,
    D16,
    R8,
    A8,
    A16,
    SPR8,
    Count
};

static constexpr EnumArray<Operand, const char*> operand_str = {"NONE", "R",   "MR", "MRI", "MRD", "MRIO",
                                                                "D8",   "D16", "R8", "A8",  "A16", "SPR8"};

enum class Mnemonic
{
    NONE,
    NOP,
    STOP,
    HALT,
    LD,
    LDH,
    INC,
    DEC,
    ADD,
    SUB,
    ADC,
    SBC,
    RLCA,
    RRCA,
    RLA,
    JR,
    JP,
    RRA,
    DAA,
    CPL,
    SCF,
    CCF,
    AND,
    XOR,
    OR,
    CP,
    RET,
    RETI,
    POP,
    PUSH,
    CALL,
    RST,
    PREFIX,
    DI,
    EI,
    Count
};

static constexpr EnumArray<Mnemonic, const char*> mnemonic_str = {
    "NONE", "NOP",  "STOP", "HALT", "LD",  "LDH",  "INC",  "DEC", "ADD",    "SUB", "ADC", "SBC",
    "RLCA", "RRCA", "RLA",  "JR",   "JP",  "RRA",  "DAA",  "CPL", "SCF",    "CCF", "AND", "XOR",
    "OR",   "CP",   "RET",  "RETI", "POP", "PUSH", "CALL", "RST", "PREFIX", "DI",  "EI",
};

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
    using Function = void (*)(Gameboy& gb, const Instr& instr);
    uint8_t opcode[2] = {0, 0};
    Mnemonic mnemonic = Mnemonic::NONE;
    Cond cond = Cond::NONE;
    Operand op1 = Operand::NONE;
    Reg reg1 = Reg::NONE;
    Operand op2 = Operand::NONE;
    Reg reg2 = Reg::NONE;
    Function func = nullptr;

    uint16_t data = 0;
    uint16_t mem_dst = 0;
    uint16_t mem_src = 0;
};

extern Instr instructions[0x100];
std::string to_string(const Gameboy& gb, const Instr& instr);


struct _Instr
{
    using Exec = uint32_t (*)(Gameboy&, const _Instr&);
    using ToString = uint32_t (*)(Gameboy&, const _Instr&, char* buf);

    Exec exec = nullptr;
    ToString to_string = nullptr;
    Reg r1 = Reg::NONE;
    Reg r2 = Reg::NONE;
    Cond cond = Cond::NONE;
};
