#include "cpu.h"

#include <cstring>

void CPU::reset()
{
    memset(this, 0, sizeof(CPU));
    pc = 0x0100;
}

uint16_t CPU::read_reg(Instruction::Register instruction_reg) const
{
    ASSERT(instruction_reg != Instruction::Register::NONE);
    switch (instruction_reg)
    {
    case Instruction::Register::A:
        return a();
    case Instruction::Register::F:
        return f();
    case Instruction::Register::B:
        return b();
    case Instruction::Register::C:
        return c();
    case Instruction::Register::D:
        return d();
    case Instruction::Register::E:
        return e();
    case Instruction::Register::H:
        return h();
    case Instruction::Register::L:
        return l();
    case Instruction::Register::AF:
        return af();
    case Instruction::Register::BC:
        return bc();
    case Instruction::Register::DE:
        return de();
    case Instruction::Register::HL:
        return hl();
    case Instruction::Register::SP:
        return sp;
    case Instruction::Register::PC:
        return pc;
    default:
        return 0;
    }
}

uint8_t& CPU::get_reg8(Instruction::Register instruction_reg)
{
    ASSERT(instruction_reg != Instruction::Register::NONE);
    static uint8_t x = 0;
    switch (instruction_reg)
    {
    case Instruction::Register::A:
        return a();
    case Instruction::Register::F:
        return f();
    case Instruction::Register::B:
        return b();
    case Instruction::Register::C:
        return c();
    case Instruction::Register::D:
        return d();
    case Instruction::Register::E:
        return e();
    case Instruction::Register::H:
        return h();
    case Instruction::Register::L:
        return l();
    default:
        return x;
    }
}

uint16_t& CPU::get_reg16(Instruction::Register instruction_reg)
{
    ASSERT(instruction_reg != Instruction::Register::NONE);
    static uint16_t x = 0;
    switch(instruction_reg)
    {
    case Instruction::Register::AF:
        return af();
    case Instruction::Register::BC:
        return bc();
    case Instruction::Register::DE:
        return de();
    case Instruction::Register::HL:
        return hl();
    case Instruction::Register::SP:
        return sp;
    case Instruction::Register::PC:
        return pc;
    default:
        return x;
    }
}


bool CPU::check_condition(Instruction::Condition condition)
{
    switch (condition)
    {
    case Instruction::Condition::NONE:
        return true;
    case Instruction::Condition::C:
        return flag_c();
    case Instruction::Condition::NC:
        return !flag_c();
    case Instruction::Condition::Z:
        return flag_z();
    case Instruction::Condition::NZ:
        return !flag_z();
    default:
        ASSERT(!"Not implemented");
        return false;
    }
}
