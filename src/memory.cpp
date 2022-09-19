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

uint8_t Memory::read(uint16_t addr) const
{
    return data[addr];
}

void Memory::write(uint16_t addr, uint8_t value)
{
    data[addr] = value;
}
