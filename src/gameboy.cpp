#include "gameboy.h"

#include <cstdio>
#include <cstring>

Gameboy::Gameboy()
{}

void Gameboy::reset()
{
    memory.reset();
    cpu.reset();
}

bool Gameboy::load_rom(const char* path)
{
    reset();
    FILE* file = nullptr;
    if (fopen_s(&file, path, "rb") != 0)
    {
        return false;
    }
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);
    fread(memory.data, 1, size, file);
    fclose(file);
    return true;
}

void Gameboy::step()
{
    Instruction instruction = fetch_instruction();
    fetch_data(instruction);
    execute_instruction(instruction);
}

void Gameboy::execute_instruction(const Instruction& instruction)
{
    if (instruction.func == nullptr)
    {
        return;
    }
    instruction.func(*this, instruction);
}

Instruction Gameboy::fetch_instruction()
{
    cpu.cycles++;
    uint8_t opcode = memory.read(cpu.pc++);

    Instruction instruction = {};
    if (opcode != 0xcb)
    {
        instruction = Instruction::instructions[opcode];
    }
    else
    {
        cpu.cycles++;
        opcode = memory.read(cpu.pc++);
        cpu.pc++;
        // FIXME
        instruction = {};
        return instruction;
    }

    if (instruction.type == Instruction::Type::NONE)
    {
        char msg[64];
        sprintf(msg, "Instruction not implemented: 0x%02x\n", opcode);
    }
    return instruction;
}

void Gameboy::fetch_data(Instruction& instruction)
{
    switch (instruction.addressing)
    {
    case Instruction::Addressing::NONE: {
        break;
    }
    case Instruction::Addressing::R: {
        instruction.data = cpu.read_reg(instruction.reg1);
        break;
    }
    case Instruction::Addressing::R_R: {
        instruction.data = cpu.read_reg(instruction.reg2);
        break;
    }
    case Instruction::Addressing::MR_R: {
        instruction.data = cpu.read_reg(instruction.reg2);
        instruction.memory_dst = cpu.read_reg(instruction.reg1);
        break;
    }
    case Instruction::Addressing::A8_R: {
        instruction.data = cpu.read_reg(instruction.reg1);
        instruction.memory_dst = memory.read(cpu.pc++);
        cpu.cycles++;
        break;
    }
    case Instruction::Addressing::A16_R: {
        instruction.data = cpu.read_reg(instruction.reg1);
        uint8_t lo = memory.read(cpu.pc++);
        uint8_t hi = memory.read(cpu.pc++);
        cpu.cycles += 2;
        instruction.memory_dst = lo | (hi << 8);
        break;
    }
    case Instruction::Addressing::MR: {
        instruction.data = memory.read(cpu.read_reg(instruction.reg1));
        cpu.cycles++;
        break;
    }
    case Instruction::Addressing::R_MR: {
        instruction.data = memory.read(cpu.read_reg(instruction.reg2));
        cpu.cycles++;
        break;
    }
    case Instruction::Addressing::R8:
    case Instruction::Addressing::D8:
    case Instruction::Addressing::R_D8: {
        instruction.data = memory.read(cpu.pc++);
        cpu.cycles++;
        break;
    }
    case Instruction::Addressing::MR_D8: {
        instruction.data = memory.read(cpu.pc++);
        cpu.cycles++;
        instruction.memory_dst = cpu.read_reg(instruction.reg1);
        break;
    }
    case Instruction::Addressing::D16:
    case Instruction::Addressing::R_D16: {
        uint8_t lo = memory.read(cpu.pc++);
        uint8_t hi = memory.read(cpu.pc++);
        cpu.cycles += 2;
        instruction.data = lo | (hi << 8);
        break;
    }
    case Instruction::Addressing::R_A8: {
        instruction.data = memory.read(Memory::IO_REG_BEGIN + memory.read(cpu.pc++));
        cpu.cycles += 2;
        break;
    }
    case Instruction::Addressing::R_A16: {
        uint8_t lo = memory.read(cpu.pc++);
        uint8_t hi = memory.read(cpu.pc++);
        cpu.cycles += 2;
        uint16_t addr = lo | (hi << 8);
        instruction.data = memory.read(addr);
        break;
    }
    case Instruction::Addressing::R_HLI: {
        instruction.data = memory.read(cpu.hl());
        cpu.cycles++;
        cpu.hl()++;
        break;
    }
    case Instruction::Addressing::HLI_R: {
        instruction.data = cpu.read_reg(instruction.reg1);
        instruction.memory_dst = cpu.hl();
        cpu.hl()++;
        break;

    }
    case Instruction::Addressing::R_HLD: {
        instruction.data = memory.read(cpu.hl());
        cpu.cycles++;
        cpu.hl()--;
        break;
    }
    case Instruction::Addressing::HLD_R: {
        instruction.data = cpu.read_reg(instruction.reg1);
        instruction.memory_dst = cpu.hl();
        cpu.hl()--;
        break;
    }
    case Instruction::Addressing::HL_SPR8: {
        uint8_t ur8 = memory.read(cpu.pc++);
        int8_t r8 = *(int8_t*)&ur8;
        instruction.data = cpu.sp + r8;
        cpu.cycles++;
        break;
    }
    default:
        ASSERT(!"Not implemented");
    }
}
