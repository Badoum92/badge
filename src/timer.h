#pragma once

#include "common.h"

struct Gameboy;

void init_timer(Gameboy& gb);
void timer_tick(uint64_t cycles);
void timer_reset_div();
