#include "interrupt.h"

#include "gameboy.h"

static constexpr uint16_t interrupt_addresses[] = {0x40, 0x48, 0x50, 0x58, 0x60};

static uint8_t* int_enable = nullptr;
static uint8_t* int_flag = nullptr;

void init_interrupts(Gameboy& gb)
{
    int_enable = &gb.memory[0xffff];
    int_flag = &gb.memory[0xff0f];
}

static size_t get_next_interrupt_index()
{
    size_t i = 0;
    for (; i < static_cast<size_t>(Interrupt::Count); ++i)
    {
        if (bit(*int_flag, i) && bit(*int_enable, i))
        {
            break;
        }
    }
    return i;
}

static void execute_interrupts(Gameboy& gb)
{
    size_t i = get_next_interrupt_index();
    if (i == static_cast<size_t>(Interrupt::Count))
    {
        return;
    }

    set_bit(*int_flag, i, 0);
    gb.cpu.ime = false;

    gb.cpu.sp -= 2;
    gb.memory.write16(gb.cpu.sp, gb.cpu.pc);
    gb.cpu.pc = interrupt_addresses[i];
    gb.cpu.cycles += 5;
}

void handle_interrupts(Gameboy& gb)
{
    if (gb.cpu.ime)
    {
        execute_interrupts(gb);
        gb.cpu.enable_interrupts = false;
    }
    if (gb.cpu.enable_interrupts)
    {
        gb.cpu.ime = true;
        gb.cpu.enable_interrupts = false;
    }
}

void request_interrupt(Interrupt interrupt)
{
    set_bit(*int_flag, static_cast<size_t>(interrupt), 1);
}

bool interrupt_enable(Interrupt interrupt)
{
    return bit(*int_enable, static_cast<size_t>(interrupt));
}

bool interrupt_pending()
{
    return *int_enable & *int_flag;
}
