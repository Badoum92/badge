#include <string>
#include <vector>
#include <chrono>

#include <bag/bag.h>
#include <bag/image.h>

#include "gameboy.h"
#include "interrupt.h"

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
bool scroll_to_pc = true;
inline static constexpr uint32_t scale = 5;

void load_instruction(Gameboy& gb, size_t addr)
{
    Instr instr = gb.fetch_instruction();
    char buf[64] = {};
    if (instr.to_string == nullptr)
    {
        sprintf(buf, "0x%04llx: UNDEFINED", addr);
    }
    else
    {
        sprintf(buf, "0x%04llx: %s", addr, instr.to_string(gb, instr).c_str());
    }
    instr_info[addr] = {buf, "", false, true};
    instr_info[addr].label = "##" + instr_info[addr].text;
}

void load_disassembly()
{
    instr_info.resize(Memory::VRAM_BEGIN);
    Gameboy g = gb;
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

void state_window()
{
    if (!ImGui::Begin("State"))
    {
        ImGui::End();
        return;
    }

    // CPU Registers

    ImGui::Text("af 0x%04x   a 0x%02x", gb.cpu.af(), gb.cpu.a());
    ImGui::Text("bc 0x%04x   b 0x%02x   c 0x%02x", gb.cpu.bc(), gb.cpu.b(), gb.cpu.c());
    ImGui::Text("de 0x%04x   d 0x%02x   e 0x%02x", gb.cpu.de(), gb.cpu.d(), gb.cpu.e());
    ImGui::Text("hl 0x%04x   h 0x%02x   l 0x%02x", gb.cpu.hl(), gb.cpu.h(), gb.cpu.l());
    ImGui::Text("sp 0x%04x   pc 0x%04x", gb.cpu.sp, gb.cpu.pc);

    ImGui::Separator();

    // CPU flags

    bool flag_z = gb.cpu.flag_z();
    bool flag_n = gb.cpu.flag_n();
    bool flag_h = gb.cpu.flag_h();
    bool flag_c = gb.cpu.flag_c();

    ImGui::TextUnformatted("z");
    ImGui::SameLine();
    ImGui::Checkbox("##z", &flag_z);
    ImGui::SameLine();
    ImGui::TextUnformatted("n");
    ImGui::SameLine();
    ImGui::Checkbox("##n", &flag_n);

    ImGui::TextUnformatted("h");
    ImGui::SameLine();
    ImGui::Checkbox("##h", &flag_h);
    ImGui::SameLine();
    ImGui::TextUnformatted("c");
    ImGui::SameLine();
    ImGui::Checkbox("##c", &flag_c);

    ImGui::Separator();

    // Interrupts

    bool ime = gb.cpu.ime;
    ImGui::TextUnformatted("IME");
    ImGui::SameLine();
    ImGui::Checkbox("##IME", &ime);

    for (size_t i = 0; i < static_cast<size_t>(Interrupt::Count); ++i)
    {
        Interrupt interrupt = static_cast<Interrupt>(i);
        bool enable = interrupt_enable(interrupt);
        char label[32];
        sprintf(label, "##%s", interrupt_str[interrupt]);
        ImGui::TextUnformatted(interrupt_str[interrupt]);
        ImGui::SameLine();
        ImGui::Checkbox(label, &enable);
    }

    ImGui::Separator();

    // Timer

    ImGui::Text("DIV 0x%02x   TIMA 0x%02x", gb.memory[0xff04], gb.memory[0xff05]);
    ImGui::Text("TMA 0x%02x   TAC 0x%02x", gb.memory[0xff06], gb.memory[0xff07]);

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
    }
    ImGui::SameLine();
    if (ImGui::Button("Step") && gb.stepping)
    {
        gb.step();
        scroll_to_pc = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Pause"))
    {
        gb.stepping = true;
        scroll_to_pc = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Goto pc"))
    {
        scroll_to_pc = true;
    }

    ImGui::Text("cycles   %llu", gb.cpu.cycles);
    if (gb.cpu.pc < instr_info.size())
    {
        ImGui::Text("%s", instr_info[gb.cpu.pc].text.c_str());
    }
    else
    {
        ImGui::Text("0x%02x", gb.memory[gb.cpu.pc]);
    }

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
            if (scroll_to_pc)
            {
                ImGui::SetScrollHereY(0.4f);
                scroll_to_pc = false;
            }
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

void serial_window()
{
    if (!ImGui::Begin("Serial"))
    {
        ImGui::End();
        return;
    }

    if (ImGui::Button("Clear"))
    {
        gb.serial_data.clear();
    }
    ImGui::BeginChild("SerialDataText");
    ImGui::TextWrapped("%s", gb.serial_data.c_str());
    ImGui::EndChild();

    ImGui::End();
}

void render()
{
    game_window();
    control_window();
    memory_window();
    state_window();
    disassembly_window();
    cart_info_window();
    serial_window();
}

void update()
{
    size_t n_instructions = 100000;
    auto begin = std::chrono::high_resolution_clock::now();

    while (!gb.stepping)
    {
        if (gb.cpu.pc < instr_info.size() && instr_info[gb.cpu.pc].breakpoint)
        {
            scroll_to_pc = true;
            gb.stepping = true;
            break;
        }
        if (n_instructions == 0)
        {
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
            if (duration >= 30)
            {
                break;
            }
            n_instructions = 100000;
        }
        gb.step();
        n_instructions--;
    }

    render();
}

int main(int argc, char** argv)
{
    bag::init(bag::Options{160 * scale, 144 * scale, "badge"}, false);
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
