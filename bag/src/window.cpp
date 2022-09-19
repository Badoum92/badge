#include "window.h"

#include <stdexcept>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "bag/bag.h"

namespace bag
{
static void framebuffer_size_callback(GLFWwindow*, int, int);
static void cursor_pos_callback(GLFWwindow*, double, double);
static void mouse_button_callback(GLFWwindow*, int, int, int);
static void key_callback(GLFWwindow*, int, int, int, int);

void Window::create(Options& options)
{
    if (handle_)
    {
        throw std::runtime_error("Tried creating 2 windows");
    }

    if (!glfwInit())
    {
        throw std::runtime_error("Could not initialize glfw");
    }

    options_ = &options;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, options.resizeable);

    handle_ = glfwCreateWindow(options.width, options.height, options.title.c_str(), nullptr, nullptr);

    glfwMakeContextCurrent(handle_);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        throw std::runtime_error("Could not initialize glad");
    }

    glfwSwapInterval(options.vsync);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glfwSetFramebufferSizeCallback(handle_, framebuffer_size_callback);
    glfwSetCursorPosCallback(handle_, cursor_pos_callback);
    glfwSetMouseButtonCallback(handle_, mouse_button_callback);
    glfwSetKeyCallback(handle_, key_callback);
}

void Window::destroy()
{
    glfwDestroyWindow(handle_);
    glfwTerminate();
    handle_ = nullptr;

}

uint32_t Window::width()
{
    return options_->width;
}

uint32_t Window::height()
{
    return options_->height;
}

void Window::resized(uint32_t width, uint32_t height)
{
    options_->width = width;
    options_->height = height;
}

void Window::poll_events()
{
    glfwPollEvents();
}

void Window::wait_events()
{
    glfwWaitEvents();
}

void Window::swap_buffers()
{
    glfwSwapBuffers(handle_);
}

void Window::close()
{
    glfwSetWindowShouldClose(handle_, GLFW_TRUE);
}

bool Window::should_close()
{
    return glfwWindowShouldClose(handle_) != 0;
}

void Window::set_title(const std::string& title)
{
    glfwSetWindowTitle(handle_, title.c_str());
    options_->title = title;
}

void Window::set_vsync(bool vsync)
{
    glfwSwapInterval(vsync);
    options_->vsync = vsync;
}

GLFWwindow* Window::handle()
{
    return handle_;
}

/* GLFW callbacks */

static void framebuffer_size_callback(GLFWwindow*, int width, int height)
{
    Window::resized(width, height);
    glViewport(0, 0, width, height);
}

static void cursor_pos_callback(GLFWwindow*, double, double)
{}

static void mouse_button_callback(GLFWwindow*, int, int, int)
{}

static void key_callback(GLFWwindow*, int key, int, int, int)
{
    if (key == GLFW_KEY_ESCAPE)
    {
        Window::close();
    }
}
} // namespace bag
