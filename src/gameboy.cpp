#include "gameboy.h"

#include <cstdio>
#include <cstring>

#include "interrupt.h"
#include "timer.h"

Gameboy::Gameboy()
{
    init_interrupts(*this);
    init_timer(*this);
}

void Gameboy::reset()
{
    memory.reset(cart_info);
    cpu.reset(cart_info);
    stepping = true;
    serial_data.clear();
}

bool Gameboy::load_rom(const char* path)
{
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
    memory.reset(cart_info);
    cpu.reset(cart_info);

    return true;
}

void Gameboy::step()
{
    if (!cpu.halted)
    {
        Instr instr = fetch_instruction();
        cpu.cycles = execute_instruction(instr);
    }
    else
    {
        cpu.cycles = 1;
        cpu.halted = !interrupt_pending();
    }

    timer_tick(cpu.cycles);
    handle_interrupts(*this);
    process_serial_data();
}

uint32_t Gameboy::execute_instruction(const Instr& instr)
{
    ASSERT(instr.exec != nullptr);
    return instr.exec(*this, instr);
}

Instr Gameboy::fetch_instruction()
{
    uint8_t opcode = memory.read(cpu.pc++);
    return instructions[opcode];
}

void Gameboy::process_serial_data()
{
    if (memory[0xff02] == 0x81)
    {
        char c = memory[0xff01];
        serial_data.push_back(c);
        memory[0xff02] = 0;
    }
}
