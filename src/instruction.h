#pragma once

#include <string>

#include "common.h"
#include "enum_array.h"

struct Gameboy;

struct Instruction
{
    enum class Type
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

    static constexpr EnumArray<Type, const char*> type_str = {
        "NONE", "NOP",  "STOP", "HALT", "LD",  "LDH",  "INC",  "DEC", "ADD",    "SUB", "ADC", "SBC",
        "RLCA", "RRCA", "RLA",  "JR",   "JP",  "RRA",  "DAA",  "CPL", "SCF",    "CCF", "AND", "XOR",
        "OR",   "CP",   "RET",  "RETI", "POP", "PUSH", "CALL", "RST", "PREFIX", "DI",  "EI"};

    enum class Addressing
    {
        NONE,
        R,
        R_R,
        MR_R,
        HLI_R,
        HLD_R,
        A8_R,
        A16_R,
        MR,
        R_MR,
        R8,
        D8,
        R_D8,
        MR_D8,
        D16,
        R_D16,
        R_A8,
        R_A16,
        R_HLI,
        R_HLD,
        HL_SPR8,
        Count
    };

    static constexpr EnumArray<Addressing, const char*> addressing_str = {
        "NONE", "R",    "R_R",   "MR_R", "HLI_R", "HLD_R", "A8_R",  "A16_R", "MR",    "R_MR",   "R8",
        "D8",   "R_D8", "MR_D8", "D16",  "R_D16", "R_A8",  "R_A16", "R_HLI", "R_HLD", "HL_SPR8"};

    enum class Register
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

    static constexpr EnumArray<Register, const char*> register_str = {"NONE", "A",  "F",  "B",  "C",  "D",  "E", "H",
                                                                      "L",    "AF", "BC", "DE", "HL", "SP", "PC"};

    enum class Condition
    {
        NONE,
        NZ,
        Z,
        NC,
        C,
        Count
    };

    static constexpr EnumArray<Register, const char*> condition_str = {"NONE", "NZ", "Z", "NC", "C"};

    static Instruction instructions[0x100];

    using Function = void (*)(Gameboy& gb, const Instruction& instruction);

    Type type = Type::NONE;
    Addressing addressing = Addressing::NONE;
    Register reg1 = Register::NONE;
    Register reg2 = Register::NONE;
    Condition cond = Condition::NONE;
    Function func = nullptr;

    uint16_t data = 0;
    uint16_t memory_dst = 0;
};

std::string to_string(const Instruction& instruction);
