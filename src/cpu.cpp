#include "cpu.h"

#include <cstring>

#include "gameboy.h"

void CPU::reset(const CartInfo& cart_info)
{
    memset(this, 0, sizeof(CPU));

    a() = 0x01;
    flag_z(1);
    flag_n(0);
    flag_h(cart_info.header_checksum != 0);
    flag_c(cart_info.header_checksum != 0);
    b() = 0x00;
    c() = 0x13;
    d() = 0x00;
    e() = 0xd8;
    h() = 0x01;
    l() = 0x4d;
    sp = 0xfffe;
    pc = 0x0100;
    ime = 0;
    enable_interrupts = false;
}

uint16_t CPU::read_reg(Reg reg) const
{
    switch (reg)
    {
    case Reg::A:
        return a();
    case Reg::F:
        return f();
    case Reg::B:
        return b();
    case Reg::C:
        return c();
    case Reg::D:
        return d();
    case Reg::E:
        return e();
    case Reg::H:
        return h();
    case Reg::L:
        return l();
    case Reg::AF:
        return af();
    case Reg::BC:
        return bc();
    case Reg::DE:
        return de();
    case Reg::HL:
        return hl();
    case Reg::SP:
        return sp;
    case Reg::PC:
        return pc;
    default:
        ASSERT(reg != Reg::NONE);
        return 0;
    }
}

uint8_t& CPU::get_reg8(Reg reg)
{
    ASSERT(reg != Reg::NONE && reg != Reg::Count);
    static uint8_t x = 0;
    switch (reg)
    {
    case Reg::A:
        return a();
    case Reg::F:
        return f();
    case Reg::B:
        return b();
    case Reg::C:
        return c();
    case Reg::D:
        return d();
    case Reg::E:
        return e();
    case Reg::H:
        return h();
    case Reg::L:
        return l();
    default:
        return x;
    }
}

uint16_t& CPU::get_reg16(Reg reg)
{
    ASSERT(reg != Reg::NONE && reg != Reg::Count);
    static uint16_t x = 0;
    switch (reg)
    {
    case Reg::AF:
        return af();
    case Reg::BC:
        return bc();
    case Reg::DE:
        return de();
    case Reg::HL:
        return hl();
    case Reg::SP:
        return sp;
    case Reg::PC:
        return pc;
    default:
        return x;
    }
}

void CPU::set_reg(Reg reg, uint16_t val)
{
    ASSERT(reg != Reg::NONE && reg != Reg::Count);
    if (reg < Reg::AF)
    {
        get_reg8(reg) = val;
    }
    else
    {
        get_reg16(reg) = val;
    }
}

bool CPU::check_condition(Cond cond) const
{
    switch (cond)
    {
    case Cond::NONE:
        return true;
    case Cond::C:
        return flag_c();
    case Cond::NC:
        return !flag_c();
    case Cond::Z:
        return flag_z();
    case Cond::NZ:
        return !flag_z();
    default:
        ASSERT(!"Not implemented");
        return false;
    }
}
