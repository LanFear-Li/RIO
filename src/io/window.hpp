#pragma once

#include <glad/glad.h>
#include "GLFW/glfw3.h"

#include <functional>

class Window final
{
public:
    Window() = delete;
    Window(int width, int height, const char *title);

    void mainLoop(const std::function<void()> &func);

    enum Mouse
    {
        MOUSE_LEFT = 1,
        MOUSE_RIGHT = 2,
        MOUSE_MIDDLE = 4,
    };

    using mouseCallback = std::function<void(GLFWwindow*, uint32_t, float, float, float, float)>;
    using keyCallback = std::function<void(GLFWwindow*, int, int)>;
    using resizeCallback = std::function<void(GLFWwindow*, uint32_t, uint32_t)>;

    void setMouseCallback(const mouseCallback &callback);
    void setKeyCallback(const keyCallback &callback);
    void setResizeCallback(const resizeCallback &callback);

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    bool firstMouse = true;
    float lastX = 0.0f;
    float lastY = 0.0f;

    uint32_t screenWidth;
    uint32_t screenHeight;

    GLFWwindow *glWindow = nullptr;

    friend void GlfwCursorPosCallback(GLFWwindow *window, double x, double y);
    friend void GlfwKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
    friend void GlfwResizeCallback(GLFWwindow *window, int width, int height);

    mouseCallback mouse_callback_ = [](GLFWwindow*, uint32_t, float, float, float, float) {};
    keyCallback key_callback_ = [](GLFWwindow*, int, int) {};
    resizeCallback resize_callback_ = [](GLFWwindow*, uint32_t, uint32_t) {};

    void switchFullScreen();
    void exit() const;

    bool isFullScreen = false;
};
