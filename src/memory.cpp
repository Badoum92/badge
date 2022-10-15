#include "memory.h"

#include <cstring>

#include "gameboy.h"
#include "timer.h"

void Memory::reset(const CartInfo&)
{
    memset(data + VRAM_BEGIN, 0, SIZE - VRAM_BEGIN);
    data[0xFF00] = 0xCF;
    data[0xFF01] = 0x00;
    data[0xFF02] = 0x7E;
    data[0xFF04] = 0xAB;
    data[0xFF05] = 0x00;
    data[0xFF06] = 0x00;
    data[0xFF07] = 0xF8;
    data[0xFF0F] = 0xE1;
    data[0xFF10] = 0x80;
    data[0xFF11] = 0xBF;
    data[0xFF12] = 0xF3;
    data[0xFF13] = 0xFF;
    data[0xFF14] = 0xBF;
    data[0xFF16] = 0x3F;
    data[0xFF17] = 0x00;
    data[0xFF18] = 0xFF;
    data[0xFF19] = 0xBF;
    data[0xFF1A] = 0x7F;
    data[0xFF1B] = 0xFF;
    data[0xFF1C] = 0x9F;
    data[0xFF1D] = 0xFF;
    data[0xFF1E] = 0xBF;
    data[0xFF20] = 0xFF;
    data[0xFF21] = 0x00;
    data[0xFF22] = 0x00;
    data[0xFF23] = 0xBF;
    data[0xFF24] = 0x77;
    data[0xFF25] = 0xF3;
    data[0xFF26] = 0xF1;
    data[0xFF40] = 0x91;
    data[0xFF41] = 0x85;
    data[0xFF42] = 0x00;
    data[0xFF43] = 0x00;
    data[0xFF44] = 0x00;
    data[0xFF45] = 0x00;
    data[0xFF46] = 0xFF;
    data[0xFF47] = 0xFC;
    data[0xFF48] = 0x00;
    data[0xFF49] = 0x00;
    data[0xFF4A] = 0x00;
    data[0xFF4B] = 0x00;
    data[0xFF4D] = 0xFF;
    data[0xFF4F] = 0xFF;
    data[0xFF51] = 0xFF;
    data[0xFF52] = 0xFF;
    data[0xFF53] = 0xFF;
    data[0xFF54] = 0xFF;
    data[0xFF55] = 0xFF;
    data[0xFF56] = 0xFF;
    data[0xFF68] = 0xFF;
    data[0xFF69] = 0xFF;
    data[0xFF6A] = 0xFF;
    data[0xFF6B] = 0xFF;
    data[0xFF70] = 0xFF;
    data[0xFFFF] = 0x00;
}

uint8_t Memory::operator[](size_t i) const
{
    ASSERT(i < SIZE);
    return data[i];
}

uint8_t& Memory::operator[](size_t i)
{
    ASSERT(i < SIZE);
    return data[i];
}

uint8_t Memory::read(uint16_t addr) const
{
    return data[addr];
}

uint16_t Memory::read16(uint16_t addr) const
{
    uint8_t lo = read(addr);
    uint8_t hi = read(addr + 1);
    return lo | (hi << 8);
}

void Memory::write(uint16_t addr, uint8_t value)
{
    if (addr <= ROM_BANK_N_END)
    {
        return;
    }

    if (addr == 0xff04)
    {
        timer_reset_div();
        return;
    }

    data[addr] = value;
}

void Memory::write16(uint16_t addr, uint16_t value)
{
    if (addr <= ROM_BANK_N_END)
    {
        return;
    }

    uint8_t lo = value & 0x00ff;
    uint8_t hi = (value & 0xff00) >> 8;
    write(addr, lo);
    write(addr + 1, hi);
}
