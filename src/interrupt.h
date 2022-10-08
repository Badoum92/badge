#pragma once

#include "common.h"
#include "enum_array.h"

struct Gameboy;

enum class Interrupt
{
    VBLANK,
    LCD_STAT,
    TIMER,
    SERIAL,
    JOYPAD,
    Count
};

static constexpr EnumArray<Interrupt, const char*> interrupt_str = {"VBLANK", "LCD_STAT", "TIMER", "SERIAL", "JOYPAD"};

void init_interrupts(Gameboy& gb);
void handle_interrupts(Gameboy& gb);
void request_interrupt(Interrupt interrupt);
bool interrupt_enable(Interrupt interrupt);
