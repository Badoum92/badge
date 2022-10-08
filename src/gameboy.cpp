#include "gameboy.h"

#include <cstdio>
#include <cstring>

#include "interrupt.h"

Gameboy::Gameboy()
{
    init_interrupts(*this);
}

void Gameboy::reset()
{
    memory.reset();
    cpu.reset(cart_info);
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
    Instr instr = fetch_instruction();
    execute_instruction(instr);
    handle_interrupts(*this);
}

void Gameboy::execute_instruction(const Instr& instr)
{
    ASSERT(instr.exec != nullptr);
    cpu.cycles += instr.exec(*this, instr);
}

Instr Gameboy::fetch_instruction()
{
    uint8_t opcode = memory.read(cpu.pc++);
    return instructions[opcode];
}
