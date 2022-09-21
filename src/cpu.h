#pragma once

#include "common.h"
#include "instruction.h"

struct CPU
{
    struct
    {
        uint8_t f;
        uint8_t a;
        uint8_t c;
        uint8_t b;
        uint8_t e;
        uint8_t d;
        uint8_t l;
        uint8_t h;
    } regs;
    uint16_t sp = 0x0000;
    uint16_t pc = 0x0100;
    uint64_t cycles = 0;

    void reset();
    uint16_t read_reg(Instruction::Register instruction_reg) const;
    uint8_t& get_reg8(Instruction::Register instruction_reg);
    uint16_t& get_reg16(Instruction::Register instruction_reg);
    bool check_condition(Instruction::Condition condition);

// clang-format off
    inline uint8_t a() const { return regs.a; }
    inline uint8_t& a() { return regs.a; }
    inline uint8_t f() const { return regs.f; }
    inline uint8_t& f() { return regs.f; }
    inline uint8_t b() const { return regs.b; }
    inline uint8_t& b() { return regs.b; }
    inline uint8_t c() const { return regs.c; }
    inline uint8_t& c() { return regs.c; }
    inline uint8_t d() const { return regs.d; }
    inline uint8_t& d() { return regs.d; }
    inline uint8_t e() const { return regs.e; }
    inline uint8_t& e() { return regs.e; }
    inline uint8_t h() const { return regs.h; }
    inline uint8_t& h() { return regs.h; }
    inline uint8_t l() const { return regs.l; }
    inline uint8_t& l() { return regs.l; }
    inline uint16_t af() const { return *(uint16_t*)&regs.f; }
    inline uint16_t& af() { return *(uint16_t*)&regs.f; }
    inline uint16_t bc() const { return *(uint16_t*)&regs.c; }
    inline uint16_t& bc() { return *(uint16_t*)&regs.c; }
    inline uint16_t de() const { return *(uint16_t*)&regs.e; }
    inline uint16_t& de() { return *(uint16_t*)&regs.e; }
    inline uint16_t hl() const { return *(uint16_t*)&regs.l; }
    inline uint16_t& hl() { return *(uint16_t*)&regs.l; }

    inline uint8_t flag_z() const { return bit(f(), 7); }
    inline void flag_z(bool b) { set_bit(f(), 7, b); }
    inline uint8_t flag_n() const { return bit(f(), 6); }
    inline void flag_n(bool b) { set_bit(f(), 6, b); }
    inline uint8_t flag_h() const { return bit(f(), 5); }
    inline void flag_h(bool b) { set_bit(f(), 5, b); }
    inline uint8_t flag_c() const { return bit(f(), 4); }
    inline void flag_c(bool b) { set_bit(f(), 4, b); }
// clang-format on
};
