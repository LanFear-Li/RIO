#include "window.hpp"

#include <iostream>

void GlfwErrorLogFunc(int error, const char *desc)
{
    std::cerr << "glfw error: " << desc << std::endl;
}

void GlfwCursorPosCallback(GLFWwindow *glfw_window, double x, double y)
{
    auto window = reinterpret_cast<Window *>(glfwGetWindowUserPointer(glfw_window));

    if (window->firstMouse){
        window->lastX = x;
        window->lastY = y;
        window->firstMouse = false;
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

    window->mouse_callback_(glfw_window, state, x, y, window->lastX, window->lastY);

    window->lastX = x;
    window->lastY = y;
}

void GlfwKeyCallback(GLFWwindow *glfw_window, int key, int scancode, int action, int mods)
{
    auto window = reinterpret_cast<Window *>(glfwGetWindowUserPointer(glfw_window));
    window->key_callback_(glfw_window, key, action);
}

void GlfwResizeCallback(GLFWwindow *glfw_window, int width, int height) {
    auto window = reinterpret_cast<Window *>(glfwGetWindowUserPointer(glfw_window));
    glViewport(0, 0, width, height);

    float width_scale, height_scale;
    glfwGetWindowContentScale(glfw_window, &width_scale, &height_scale);

    width /= width_scale;
    height /= height_scale;
    if (window->screenWidth != width || window->screenHeight != height) {
        window->resize_callback_(glfw_window, width, height);
    }
}

Window::Window(int width, int height, const char *title) : screenWidth(width), screenHeight(height)
{
    glfwSetErrorCallback(GlfwErrorLogFunc);
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    glWindow = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (glWindow == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(glWindow);

    glfwSetWindowUserPointer(glWindow, this);

    glfwSetKeyCallback(glWindow, GlfwKeyCallback);
    glfwSetCursorPosCallback(glWindow, GlfwCursorPosCallback);
    glfwSetFramebufferSizeCallback(glWindow, GlfwResizeCallback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    // Bind framebuffer resolution.
    int frame_width, frame_height;
    glfwGetFramebufferSize(glWindow, &frame_width, &frame_height);
    glViewport(0, 0, frame_width, frame_height);
}

void Window::mainLoop(const std::function<void()> &func)
{
    while (!glfwWindowShouldClose(glWindow)) {
        glfwPollEvents();

        auto currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        func();

        glfwSwapBuffers(glWindow);
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
