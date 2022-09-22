#include "gameboy.h"

#include <cstdio>
#include <cstring>

Gameboy::Gameboy()
{}

void Gameboy::reset()
{
}

bool Gameboy::load_rom(const char* path)
{
    memory.reset();
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

    for (size_t i = 0; i < 16; ++i)
    {
        char c = memory.read(0x134 + i);
        if (c < 'A' || c > 'Z')
        {
            break;
        }
        cart_info.title[i] = c;
    }

    cart_info.cgb = memory[0x143];
    if (cart_info.cgb != 0x80 && cart_info.cgb != 0xc0)
    {
        cart_info.cgb = 0;
    }

    cart_info.sgb = memory[0x146];
    cart_info.cartridge_type = memory[0x147];
    cart_info.rom_size = memory[0x148];
    cart_info.ram_size = memory[0x149];
    cart_info.header_checksum = memory[0x14d];
    cpu.reset(cart_info);

    return true;
}

void Gameboy::step()
{
    // uint16_t addr = cpu.pc;
    Instr instr = fetch_instruction();
    fetch_data(instr);

    // printf("0x%04x: %s\n", addr, to_string(*this, instr).c_str());

    execute_instruction(instr);
}

void Gameboy::execute_instruction(const Instr& instr)
{
    if (instr.func == nullptr)
    {
        char buf[64];
        sprintf(buf, "Instruction not implemented 0x%02x 0x%02x", instr.opcode[0], instr.opcode[1]);
        ASSERT_MSG(instr.func != nullptr, buf);
    }
    instr.func(*this, instr);
}

Instr Gameboy::fetch_instruction()
{
    uint8_t opcode = memory.read(cpu.pc++);
    Instr instr = {};
    if (opcode != 0xcb)
    {
        instr = instructions[opcode];
    }
    else
    {
        opcode = memory.read(cpu.pc++);
        // FIXME
        cpu.pc++;
        instr = {};
        return instr;
    }
    return instr;
}

void Gameboy::fetch_data(Instr& instr)
{
    size_t n_operand = 0;
    Operand data_operand = Operand::NONE;
    Reg data_reg = Reg::NONE;

    if (instr.op1 != Operand::NONE && instr.op2 != Operand::NONE)
    {
        n_operand = 2;
        data_operand = instr.op2;
        data_reg = instr.reg2;
        switch (instr.op1)
        {
        case Operand::MR:
            instr.mem_dst = cpu.read_reg(instr.reg1);
            break;
        case Operand::MRI:
            instr.mem_dst = cpu.read_reg(instr.reg1);
            cpu.hl()++;
            break;
        case Operand::MRD:
            instr.mem_dst = cpu.read_reg(instr.reg1);
            cpu.hl()--;
            break;
        case Operand::MRIO:
            instr.mem_dst = Memory::IO_REG_BEGIN + cpu.read_reg(instr.reg1);
            break;
        case Operand::A8:
            instr.mem_dst = Memory::IO_REG_BEGIN + memory.read(cpu.pc++);
            break;
        case Operand::A16: {
            uint8_t lo = memory.read(cpu.pc++);
            uint8_t hi = memory.read(cpu.pc++);
            instr.mem_dst = lo | (hi << 8);
            break;
        }
        default:
            break;
        }
    }

    if (instr.op1 != Operand::NONE && data_operand == Operand::NONE)
    {
        n_operand = 1;
        data_operand = instr.op1;
        data_reg = instr.reg1;
    }

    if (data_operand == Operand::NONE)
    {
        return;
    }

    switch (data_operand)
    {
    case Operand::R:
        instr.data = cpu.read_reg(data_reg);
        break;
    case Operand::MR: {
        instr.data = memory.read(cpu.read_reg(data_reg));
        if (n_operand == 1)
        {
            instr.mem_dst = cpu.read_reg(data_reg);
        }
        break;
    }
    case Operand::MRI:
        instr.data = memory.read(cpu.read_reg(data_reg));
        cpu.hl()++;
        break;
    case Operand::MRD:
        instr.data = memory.read(cpu.read_reg(data_reg));
        cpu.hl()--;
        break;
    case Operand::MRIO:
        instr.data = memory.read(Memory::IO_REG_BEGIN + cpu.read_reg(data_reg));
        break;
    case Operand::D8:
        instr.data = memory.read(cpu.pc++);
        break;
    case Operand::D16: {
        uint8_t lo = memory.read(cpu.pc++);
        uint8_t hi = memory.read(cpu.pc++);
        instr.data = lo | (hi << 8);
        break;
    }
    case Operand::R8:
        instr.data = memory.read(cpu.pc++);
        break;
    case Operand::A8:
        instr.mem_src = Memory::IO_REG_BEGIN + memory.read(cpu.pc++);
        instr.data = memory.read(instr.mem_src);
        break;
    case Operand::A16: {
        uint8_t lo = memory.read(cpu.pc++);
        uint8_t hi = memory.read(cpu.pc++);
        instr.mem_src = lo | (hi << 8);
        instr.data = memory.read(instr.mem_src);
        break;
    }
    case Operand::SPR8:
        instr.data = memory.read(cpu.pc++);
        break;
    default:
        break;
    }
}
