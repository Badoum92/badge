#include "bag.h"

#include <cassert>
#include <glad/glad.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>
#include <GLFW/glfw3.h>

#include "window.h"

namespace bag
{
static Options options_;
static bool full_window_imgui_ = false;

void init(const Options& options, bool full_window_imgui)
{
    options_ = options;
    full_window_imgui_ = full_window_imgui;

    Window::create(options_);

    IMGUI_CHECKVERSION();

    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;
    ImGui::GetIO().ConfigWindowsResizeFromEdges = true;
    bool imgui_glfw_init = ImGui_ImplGlfw_InitForOpenGL(Window::handle(), true);
    assert(imgui_glfw_init);
    bool imgui_opengl3_init = ImGui_ImplOpenGL3_Init("#version 450 core");
    assert(imgui_opengl3_init);
}

void terminate()
{
    ImGui::DestroyContext();
    Window::destroy();
}

void begin_frame()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (full_window_imgui_)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);

        static ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration
            | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus
            | ImGuiWindowFlags_NoBackground;
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        if (!ImGui::Begin("Fullscreen window", nullptr, flags))
        {
            ImGui::PopStyleVar(2);
            ImGui::End();
            ImGui::EndFrame();
            return;
        }
        ImGui::PopStyleVar(2);
    }
}

void end_frame()
{
    if (full_window_imgui_)
    {
        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(context);
    }

    Window::swap_buffers();
    Window::poll_events();
}

bool running()
{
    return !Window::should_close();
}

const Options& options()
{
    return options_;
}
} // namespace bag
