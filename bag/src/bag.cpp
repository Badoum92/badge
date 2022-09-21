#include "bag.h"

#include <glad/glad.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>

#include "window.h"

namespace bag
{
Application::Application(const Options& options, bool full_window_imgui)
{
    options_ = options;
    full_window_imgui_ = full_window_imgui;

    Window::create(options_);

    IMGUI_CHECKVERSION();

    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::GetIO().ConfigWindowsResizeFromEdges = true;
    ImGui_ImplGlfw_InitForOpenGL(Window::handle(), true);
    ImGui_ImplOpenGL3_Init("#version 450 core");
}

Application::~Application()
{
    ImGui::DestroyContext();
    Window::destroy();
}

void Application::run()
{
    while (!Window::should_close())
    {
        render();
        Window::swap_buffers();
        Window::poll_events();
    }
}

void Application::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (full_window_imgui_)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);

        static bool use_work_area = false;
        static ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove
            | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoBackground;
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(use_work_area ? viewport->WorkPos : viewport->Pos);
        ImGui::SetNextWindowSize(use_work_area ? viewport->WorkSize : viewport->Size);
        if (!ImGui::Begin("Fullscreen window", nullptr, flags))
        {
            ImGui::PopStyleVar(2);
            ImGui::EndFrame();
            return;
        }
        ImGui::PopStyleVar(2);
    }

    update(options_);

    if (full_window_imgui_)
    {
        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    ImGui::EndFrame();
}
} // namespace bag
