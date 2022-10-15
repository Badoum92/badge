#include "timer.h"

#include "gameboy.h"
#include "interrupt.h"

static uint8_t* timer_div = nullptr;
static uint8_t* timer_tima = nullptr;
static uint8_t* timer_tma = nullptr;
static uint8_t* timer_tac = nullptr;

static uint16_t internal_div = 0;
static uint64_t internal_ticks = 0;

void init_timer(Gameboy& gb)
{
    timer_div = &gb.memory[0xff04];
    timer_tima = &gb.memory[0xff05];
    timer_tma = &gb.memory[0xff06];
    timer_tac = &gb.memory[0xff07];
}

void timer_tick(uint64_t cycles)
{
    internal_div += cycles;
    if (internal_div >= 256)
    {
        internal_div -= 256;
        *timer_div += 1;
    }

    if (*timer_tac & (1 << 2))
    {
        internal_ticks += cycles * 4;

        uint64_t freq = 4096;
        if ((*timer_tac & 0b11) == 0b01)
        {
            freq = 262144;
        }
        else if ((*timer_tac & 0b11) == 0b10)
        {
            freq = 65536;
        }
        else if ((*timer_tac & 0b11) == 0b11)
        {
            freq = 16384;
        }

        while (internal_ticks >= 4194304 / freq)
        {
            *timer_tima += 1;
            if (*timer_tima == 0)
            {
                *timer_tima = *timer_tma;
                request_interrupt(Interrupt::TIMER);
            }
            internal_ticks -= 4194304 / freq;
        }
    }
}

void timer_reset_div()
{
    *timer_div = 0;
    internal_div = 0;
}
