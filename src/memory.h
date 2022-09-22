#pragma once

#include "common.h"

struct Memory
{
    static constexpr size_t SIZE = 0x10000;

    static constexpr uint16_t ROM_BANK_0_BEGIN = 0x0000;
    static constexpr uint16_t ROM_BANK_0_END = 0x3fff;

    static constexpr uint16_t ROM_BANK_N_BEGIN = 0x4000;
    static constexpr uint16_t ROM_BANK_N_END = 0x7fff;

    static constexpr uint16_t VRAM_BEGIN = 0x8000;
    static constexpr uint16_t VRAM_END = 0x9fff;

    static constexpr uint16_t EXTERN_RAM_BEGIN = 0xa000;
    static constexpr uint16_t EXTERN_RAM_END = 0xbfff;

    static constexpr uint16_t WRAM_0_BEGIN = 0xc000;
    static constexpr uint16_t WRAM_0_END = 0xcfff;

    static constexpr uint16_t WRAM_1_BEGIN = 0xd000;
    static constexpr uint16_t WRAM_1_END = 0xdfff;

    static constexpr uint16_t WRAM_MIRROR_BEGIN = 0xe000;
    static constexpr uint16_t WRAM_MIRROR_END = 0xfdff;

    static constexpr uint16_t OAM_BEGIN = 0xfe00;
    static constexpr uint16_t OAM_END = 0xfe9f;

    static constexpr uint16_t UNUSED_BEGIN = 0xfea0;
    static constexpr uint16_t UNUSED_END = 0xfeff;

    static constexpr uint16_t IO_REG_BEGIN = 0xff00;
    static constexpr uint16_t IO_REG_END = 0xff7f;

    static constexpr uint16_t HRAM_BEGIN = 0xff80;
    static constexpr uint16_t HRAM_END = 0xfffe;

    static constexpr uint16_t IE = 0xffff;

    uint8_t data[SIZE] = {};

    void reset();

    uint8_t operator[](size_t i) const;
    uint8_t& operator[](size_t i);
    uint8_t read(uint16_t addr) const;
    uint16_t read16(uint16_t addr) const;
    void write(uint16_t addr, uint8_t value);
    void write16(uint16_t addr, uint16_t value);
};
