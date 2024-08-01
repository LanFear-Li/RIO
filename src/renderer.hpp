#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "filesystem.hpp"
#include "shader.hpp"
#include "camera.hpp"
#include "model.hpp"

#include <iostream>

#include "render/pass.hpp"
#include "scene/scene.hpp"
#include "io/window.hpp"

// default settings
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 800;

const unsigned int SHADOW_WIDTH = 1024;
const unsigned int SHADOW_HEIGHT = 1024;

class Renderer
{
public:
    Renderer();

    void load();
    void run();
    static void exit();

private:
    void register_callback();

    void create_depth_map();

    void render_plane();

    void render_cube();

    void render_scene(Shader &shader);

    Window window_render;
    Scene scene;
    Pass pass;

    // shadow object
    unsigned int depthMapFBO{}, depthMap{};

    // scene buffer object
    unsigned int lightVBO{}, lightVAO{};
    unsigned int cubeVBO{}, cubeVAO{};
    unsigned int planeVBO{}, planeVAO{};

    // timing
    float moveSpeed = 1.0f;

    bool upKeyPressed = false;
    bool downKeyPressed = false;
};
