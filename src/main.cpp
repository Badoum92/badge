#include <iostream>

#include <bag/bag.h>
#include <bag/image.h>

#include "debug.h"
#include "gameboy.h"

class App : public bag::Application
{
public:
    App(int argc, char** argv)
        : bag::Application(bag::Options{.title = "bgb"}, true)
    {
        placeholder.from_file("../gameboy.jpg");
        ASSERT(argc > 1);
        gb.load_rom(argv[1]);
    }

    void game_window()
    {
        static ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration
            | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground;
        static float scale = 4;
        ImGui::SetNextWindowSize({160 * scale, 144 * scale});

        if (!ImGui::Begin("Game", nullptr, flags))
        {
            ImGui::End();
            return;
        }

        ImGui::Image(placeholder.to_ptr(), ImGui::GetWindowSize());

        ImGui::End();
    }

    virtual void update(const bag::Options&) override
    {
        game_window();
        memory_window(gb);
        registers_window(gb);
        control_window(gb);
    }

    Gameboy gb;
    bag::Image placeholder;
};

int main(int argc, char** argv)
{
    App app(argc, argv);
    app.run();
}
