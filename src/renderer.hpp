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
    void run();

private:
    Window window_render;
    Scene scene;
    Pass pass;
};
