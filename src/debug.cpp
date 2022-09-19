#include "debug.h"

#include <bag/bag.h>

void memory_window(const Gameboy& gb)
{
    static auto mem_line = [&](size_t i) {
        ImGui::Text("0x%04zx:  %02x %02x %02x %02x %02x %02x %02x %02x  %02x %02x %02x %02x %02x %02x %02x %02x", i,
                    gb.memory[i + 0], gb.memory[i + 1], gb.memory[i + 2], gb.memory[i + 3], gb.memory[i + 4],
                    gb.memory[i + 5], gb.memory[i + 6], gb.memory[i + 7], gb.memory[i + 8], gb.memory[i + 9],
                    gb.memory[i + 10], gb.memory[i + 11], gb.memory[i + 12], gb.memory[i + 13], gb.memory[i + 14],
                    gb.memory[i + 15]);
    };

    static auto mem_text = [&](uint16_t begin, uint16_t end) {
        for (size_t i = begin; i < end; i += 16)
        {
            mem_line(i);
        }
    };

    if (!ImGui::Begin("Memory"))
    {
        ImGui::End();
        return;
    }

    if (ImGui::CollapsingHeader("ROM bank 0"))
    {
        mem_text(Memory::ROM_BANK_0_BEGIN, Memory::ROM_BANK_0_END);
    }
    if (ImGui::CollapsingHeader("ROM bank N"))
    {
        mem_text(Memory::ROM_BANK_N_BEGIN, Memory::ROM_BANK_N_END);
    }
    if (ImGui::CollapsingHeader("VRAM"))
    {
        mem_text(Memory::VRAM_BEGIN, Memory::VRAM_END);
    }
    if (ImGui::CollapsingHeader("External RAM"))
    {
        mem_text(Memory::EXTERN_RAM_BEGIN, Memory::EXTERN_RAM_END);
    }
    if (ImGui::CollapsingHeader("WRAM 0"))
    {
        mem_text(Memory::WRAM_0_BEGIN, Memory::WRAM_0_END);
    }
    if (ImGui::CollapsingHeader("WRAM 1"))
    {
        mem_text(Memory::WRAM_1_BEGIN, Memory::WRAM_1_END);
    }
    if (ImGui::CollapsingHeader("WRAM mirror"))
    {
        mem_text(Memory::WRAM_MIRROR_BEGIN, Memory::WRAM_MIRROR_END);
    }
    if (ImGui::CollapsingHeader("OAM"))
    {
        mem_text(Memory::OAM_BEGIN, Memory::OAM_END);
    }
    if (ImGui::CollapsingHeader("UNUSED"))
    {
        mem_text(Memory::UNUSED_BEGIN, Memory::UNUSED_END);
    }
    if (ImGui::CollapsingHeader("IO registers"))
    {
        mem_text(Memory::IO_REG_BEGIN, Memory::IO_REG_END);
    }
    if (ImGui::CollapsingHeader("HRAM"))
    {
        mem_text(Memory::HRAM_BEGIN, Memory::HRAM_END + 1);
    }

    ImGui::End();
}

void registers_window(const Gameboy& gb)
{
    if (!ImGui::Begin("Registers"))
    {
        ImGui::End();
        return;
    }

    ImGui::Text("af 0x%04x   a 0x%02x", gb.cpu.af(), gb.cpu.a());
    ImGui::Text("bc 0x%04x   b 0x%02x   c 0x%02x", gb.cpu.bc(), gb.cpu.b(), gb.cpu.c());
    ImGui::Text("de 0x%04x   d 0x%02x   e 0x%02x", gb.cpu.de(), gb.cpu.d(), gb.cpu.e());
    ImGui::Text("hl 0x%04x   h 0x%02x   l 0x%02x", gb.cpu.hl(), gb.cpu.h(), gb.cpu.l());

    ImGui::Separator();

    ImGui::Text("z %d   n %d", gb.cpu.flag_z(), gb.cpu.flag_n());
    ImGui::Text("h %d   c %d", gb.cpu.flag_h(), gb.cpu.flag_c());

    ImGui::Separator();

    ImGui::Text("sp 0x%04x   pc 0x%04x", gb.cpu.sp, gb.cpu.pc);

    ImGui::End();
}

void control_window(Gameboy& gb)
{
    if (!ImGui::Begin("Control"))
    {
        ImGui::End();
        return;
    }

    ImGui::Button("Run");

    ImGui::SameLine();

    if (ImGui::Button("Step"))
    {
        gb.step();
    }

    ImGui::End();
}
