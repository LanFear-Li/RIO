#pragma once

#include <glad/glad.h>
#include "GLFW/glfw3.h"

#include <functional>
#include <memory>

class Window final
{
public:
    Window() = delete;
    Window(std::shared_ptr<uint32_t> width, std::shared_ptr<uint32_t> height, const char *title);

    void main_loop(const std::function<void()> &func);

    enum Mouse
    {
        MOUSE_LEFT = 1,
        MOUSE_RIGHT = 2,
        MOUSE_MIDDLE = 4,
    };

    using mouse_callback = std::function<void(GLFWwindow*, uint32_t, float, float, float, float)>;
    using key_callback = std::function<void(GLFWwindow*, int, int)>;
    using resize_callback = std::function<void(GLFWwindow*, uint32_t, uint32_t)>;

    void set_mouse_callback(const mouse_callback &callback);
    void set_key_callback(const key_callback &callback);
    void set_resize_callback(const resize_callback &callback);

    friend void glfw_cursor_pos_callback(GLFWwindow *window, double x, double y);
    friend void glfw_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
    friend void glfw_resize_callback(GLFWwindow *window, int width, int height);

    mouse_callback mouse_callback_ = [](GLFWwindow*, uint32_t, float, float, float, float) {};
    key_callback key_callback_ = [](GLFWwindow*, int, int) {};
    resize_callback resize_callback_ = [](GLFWwindow*, uint32_t, uint32_t) {};

    void switch_full_screen();
    void exit() const;

    float delta_time = 0.0f;
    float last_frame = 0.0f;

    std::shared_ptr<uint32_t> window_width;
    std::shared_ptr<uint32_t> window_height;

    GLFWwindow *gl_window = nullptr;

private:
    bool first_mouse = true;
    float last_x = 0.0f;
    float last_y = 0.0f;

    bool full_screen = false;
};
