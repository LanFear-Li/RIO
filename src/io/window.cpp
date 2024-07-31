#include "window.hpp"

#include <iostream>

void GlfwErrorLogFunc(int error, const char *desc) {
    std::cerr << "glfw error: " << desc << std::endl;
}

void GlfwCursorPosCallback(GLFWwindow *glfw_window, double x, double y)
{
    // TODO: Bind mouse callback.
}

void GlfwKeyCallback(GLFWwindow *glfw_window, int key, int scancode, int action, int mods)
{
    auto window = reinterpret_cast<Window *>(glfwGetWindowUserPointer(glfw_window));
    window->key_callback_(glfw_window, key, action);
}

Window::Window(int width, int height, const char *title)
{
    glfwSetErrorCallback(GlfwErrorLogFunc);
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    gl_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (gl_window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(gl_window);

    glfwSetWindowUserPointer(gl_window, this);

    glfwSetKeyCallback(gl_window, GlfwKeyCallback);
    // glfwSetCursorPosCallback(gl_window, GlfwCursorPosCallback);
    // glfwSetFramebufferSizeCallback(gl_window, GlfwResizeCallback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(gl_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }
}

void Window::mainLoop(const std::function<void()> &func)
{
    while (!glfwWindowShouldClose(gl_window)) {
        glfwPollEvents();

        auto currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        func();

        glfwSwapBuffers(gl_window);
    }
}

void Window::setMouseCallback(const mouseCallback &callback)
{
    mouse_callback_ = callback;
}

void Window::setKeyCallback(const keyCallback &callback)
{
    key_callback_ = callback;
}

void Window::setResizeCallback(const resizeCallback &callback)
{
    resize_callback_ = callback;
}
