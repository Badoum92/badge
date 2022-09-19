#include "cpu.h"

#include <cstring>

void CPU::reset()
{
    memset(this, 0, sizeof(CPU));
    pc = 0x0100;
}

uint16_t CPU::read_reg(Instruction::Register instruction_reg)
{
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
        ASSERT(!"Not implemented");
    }
    return 0;
}
