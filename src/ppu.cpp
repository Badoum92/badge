#include "ppu.h"

#include "gameboy.h"

void PPU::reset(Gameboy& gb)
{
    OAM_table = (OAMEntry*)&gb.memory[Memory::OAM_BEGIN];
    vram = &gb.memory[Memory::VRAM_BEGIN];
}
