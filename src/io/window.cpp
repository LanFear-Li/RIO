#include "window.hpp"

#include <iostream>

void GlfwErrorLogFunc(int error, const char *desc)
{
    std::cerr << "glfw error: " << desc << std::endl;
}

void glfw_cursor_pos_callback(GLFWwindow *glfw_window, double x, double y)
{
    auto window = reinterpret_cast<Window *>(glfwGetWindowUserPointer(glfw_window));

    if (window->first_mouse){
        window->last_x = x;
        window->last_y = y;
        window->first_mouse = false;
    }

    uint32_t state = 0;
    if (glfwGetMouseButton(glfw_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        state |= Window::MOUSE_LEFT;
    }
    if (glfwGetMouseButton(glfw_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        state |= Window::MOUSE_RIGHT;
    }
    if (glfwGetMouseButton(glfw_window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
        state |= Window::MOUSE_MIDDLE;
    }

    window->mouse_callback_(glfw_window, state, x, y, window->last_x, window->last_y);

    window->last_x = x;
    window->last_y = y;
}

void glfw_key_callback(GLFWwindow *glfw_window, int key, int scancode, int action, int mods)
{
    auto window = reinterpret_cast<Window *>(glfwGetWindowUserPointer(glfw_window));
    window->key_callback_(glfw_window, key, action);
}

void glfw_resize_callback(GLFWwindow *glfw_window, int width, int height) {
    auto window = reinterpret_cast<Window *>(glfwGetWindowUserPointer(glfw_window));
    glViewport(0, 0, width, height);

    if (*window->window_width != width || *window->window_height != height) {
        window->resize_callback_(glfw_window, width, height);
    }
}

Window::Window(std::shared_ptr<uint32_t> width, std::shared_ptr<uint32_t> height, const char *title)
    : window_width(width), window_height(height)
{
    glfwSetErrorCallback(GlfwErrorLogFunc);
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Glfw window creation.
    gl_window = glfwCreateWindow(*width, *height, title, nullptr, nullptr);
    if (gl_window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(gl_window);
    glfwSwapInterval(0);

    glfwSetWindowUserPointer(gl_window, this);

    glfwSetKeyCallback(gl_window, glfw_key_callback);
    glfwSetCursorPosCallback(gl_window, glfw_cursor_pos_callback);
    glfwSetFramebufferSizeCallback(gl_window, glfw_resize_callback);

    // Tell GLFW to capture our mouse.
    glfwSetInputMode(gl_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    // Bind framebuffer resolution.
    int frame_width, frame_height;
    glfwGetFramebufferSize(gl_window, &frame_width, &frame_height);
    glViewport(0, 0, frame_width, frame_height);
}

void Window::main_loop(const std::function<void()> &func)
{
    while (!glfwWindowShouldClose(gl_window)) {
        glfwPollEvents();

        auto currentFrame = static_cast<float>(glfwGetTime());
        delta_time = currentFrame - last_frame;
        last_frame = currentFrame;

        func();

        glfwSwapBuffers(gl_window);
    }
}

void Window::set_mouse_callback(const mouse_callback &callback)
{
    mouse_callback_ = callback;
}

void Window::set_key_callback(const key_callback &callback)
{
    key_callback_ = callback;
}

void Window::set_resize_callback(const resize_callback &callback)
{
    resize_callback_ = callback;
}

void Window::switch_full_screen()
{
    full_screen = !full_screen;

    if (full_screen) {
        glfwMaximizeWindow(gl_window);
    } else {
        glfwRestoreWindow(gl_window);
    }
}

void Window::exit() const
{
    glfwSetWindowShouldClose(gl_window, GLFW_TRUE);
}
