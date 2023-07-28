#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "filesystem.hpp"
#include "shader.hpp"
#include "camera.hpp"
#include "model.hpp"

#include <iostream>

// settings
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 800;

class Renderer
{
public:
    Renderer();

    void config();
    void load();
    void run();
    static void exit();

private:
    void processInput(GLFWwindow *window);

    void register_rx();
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void mouse_cursor_callback(GLFWwindow* window, double xpos, double ypos);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

    // window
    GLFWwindow* window{};

    // camera
    Camera *camera{};

    Shader *shader{};
    Model *model{};

    float lastX = SCR_WIDTH / 2.0f;
    float lastY = SCR_HEIGHT / 2.0f;

    // timing
    float moveSpeed = 1.0f;
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    // mouse
    bool firstMouse = true;
    bool rightMousePressd = false;

    bool upKeyPressed = false;
    bool downKeyPressed = false;
};
