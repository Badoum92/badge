#include "dma.h"

#include "gameboy.h"

void DMA::start(uint8_t byte)
{
    start_byte = byte;
    current_byte = 0;
    delay = 2;
    active = false;
}

void DMA::tick(uint64_t cycles, Gameboy& gb)
{
    if (!active)
    {
        return;
    }

    for (size_t i = 0; i < cycles; ++i)
    {
        if (delay > 0)
        {
            --delay;
            continue;
        }

        uint8_t data = gb.memory.read(start_byte * 0x100 + current_byte);
        gb.memory.write(0xfe00 + current_byte, data);

        if (current_byte == 0xa0)
        {
            active = false;
            return;
        }
    }
}
