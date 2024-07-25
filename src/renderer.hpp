#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// #include "imgui.h"
// #include "imgui_impl_glfw.h"
// #include "imgui_impl_opengl3.h"

#include "filesystem.hpp"
#include "shader.hpp"
#include "camera.hpp"
#include "model.hpp"

#include <iostream>

// settings
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 800;

const unsigned int SHADOW_WIDTH = 1024;
const unsigned int SHADOW_HEIGHT = 1024;

extern float lastX;
extern float lastY;

// mouse
extern bool firstMouse;
extern bool rightMousePressd;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void mouse_cursor_callback(GLFWwindow* window, double xpos, double ypos);

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

class Renderer
{
public:
    Renderer();

    void config();
    void load();
    void run();
    static void exit();

private:
    void process_input(GLFWwindow *window);

    void register_callback();

    void create_depth_map();

    void render_plane();

    void render_cube();

    void render_scene(Shader &shader);

    GLFWwindow* window{};

    Shader *shader{};
    Shader *depthShader{};
    Shader *lightShader{};

    // models and textures
    Model *model{};
    unsigned int woodTexture{};

    // shadow object
    unsigned int depthMapFBO{}, depthMap{};

    // scene buffer object
    unsigned int lightVBO{}, lightVAO{};
    unsigned int cubeVBO{}, cubeVAO{};
    unsigned int planeVBO{}, planeVAO{};

    // timing
    float moveSpeed = 1.0f;
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    bool upKeyPressed = false;
    bool downKeyPressed = false;
};
