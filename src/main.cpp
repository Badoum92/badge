#include <string>
#include <vector>

#include <bag/bag.h>
#include <bag/image.h>

#include "gameboy.h"

struct InstrInfo
{
    std::string text = "";
    std::string label = "";
    bool breakpoint = false;
    bool valid = false;
};

Gameboy gb;
bag::Image placeholder;
std::vector<InstrInfo> instr_info;
inline static constexpr uint32_t scale = 5;

void load_instruction(Gameboy& gb, size_t addr)
{
    Instr instr = gb.fetch_instruction();
    gb.fetch_data(instr);
    char buf[64] = {};
    sprintf(buf, "0x%04llx: %s", addr, to_string(gb, instr).c_str());
    instr_info[addr] = {buf, "", false, true};
    instr_info[addr].label = "##" + instr_info[addr].text;
}

void load_disassembly()
{
    instr_info.resize(Memory::VRAM_BEGIN);
    Gameboy g;
    memcpy(&g, &gb, sizeof(Gameboy));
    load_instruction(g, 0x100);
    load_instruction(g, 0x101);
    g.cpu.pc = 0x150;
    for (size_t i = g.cpu.pc; i < Memory::VRAM_BEGIN; i = g.cpu.pc)
    {
        load_instruction(g, i);
    }
}

void game_window()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);

    static ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration
        | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings
        | ImGuiWindowFlags_NoBackground;
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->Pos);
    ImGui::SetNextWindowSize({160.0f * scale, 144.0f * scale});

    if (!ImGui::Begin("Game", nullptr, flags))
    {
        ImGui::PopStyleVar(2);
        ImGui::End();
        return;
    }

    ImGui::Image(placeholder.to_ptr(), ImGui::GetWindowSize());

    ImGui::PopStyleVar(2);
    ImGui::End();
}

void memory_window()
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

void registers_window()
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

void control_window()
{
    if (!ImGui::Begin("Control"))
    {
        ImGui::End();
        return;
    }

    if (ImGui::Button("Run") && gb.stepping)
    {
        gb.stepping = false;
        gb.step();
    }
    ImGui::SameLine();
    if (ImGui::Button("Step") && gb.stepping)
    {
        gb.step();
    }
    ImGui::SameLine();
    if (ImGui::Button("Pause"))
    {
        gb.stepping = true;
    }

    ImGui::Text("%s", instr_info[gb.cpu.pc].text.c_str());

    ImGui::End();
}

void disassembly_window()
{
    if (!ImGui::Begin("Disassembly"))
    {
        ImGui::End();
        return;
    }

    for (size_t i = 0; i < Memory::VRAM_BEGIN; ++i)
    {
        auto& instr = instr_info[i];
        if (!instr.valid)
        {
            continue;
        }
        ImVec4 color = {1.0f, 1.0f, 1.0f, 1.0f};
        if (i == gb.cpu.pc)
        {
            color = {0.6f, 1.0f, 0.6f, 1.0f};
        }
        ImGui::Checkbox(instr.label.c_str(), &instr.breakpoint);
        ImGui::SameLine();
        ImGui::TextColored(color, "%s", instr.text.c_str());
    }

    ImGui::End();
}

void cart_info_window()
{
    if (!ImGui::Begin("Game info"))
    {
        ImGui::End();
        return;
    }

    ImGui::Text("Title: %s", gb.cart_info.title);
    ImGui::Text("cgb: 0x%02x   sgb: 0x%02x   type: 0x%02x", gb.cart_info.cgb, gb.cart_info.sgb,
                gb.cart_info.cartridge_type);
    ImGui::Text("rom size: 0x%02x   ram size: 0x%02x", gb.cart_info.rom_size, gb.cart_info.ram_size);
    ImGui::Text("header checksum: 0x%02x", gb.cart_info.header_checksum);

    ImGui::End();
}

void render()
{
    game_window();
    control_window();
    memory_window();
    registers_window();
    disassembly_window();
    cart_info_window();
}

void update()
{
    size_t n_instructions = 64;

    while (!gb.stepping && n_instructions > 0)
    {
        if (instr_info[gb.cpu.pc].breakpoint)
        {
            gb.stepping = true;
            break;
        }
        gb.step();
        n_instructions--;
    }

    render();
}

int main(int argc, char** argv)
{
    bag::init(bag::Options{160 * scale, 144 * scale, "badge", false, false}, false);
    placeholder.from_file("../gameboy.jpg");
    ASSERT(argc > 1);
    gb.load_rom(argv[1]);
    load_disassembly();

    while (bag::running())
    {
        bag::begin_frame();
        update();
        bag::end_frame();
    }

    bag::terminate();
}
