#include "memory.h"

#include <cstring>

void Memory::reset()
{
    memset(data, 0, SIZE);
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
    ASSERT(addr > ROM_BANK_N_END);
    data[addr] = value;
}

void Memory::write16(uint16_t addr, uint16_t value)
{
    uint8_t lo = value & 0x00ff;
    uint8_t hi = (value & 0xff00) >> 8;
    write(addr, lo);
    write(addr + 1, hi);
}
