#pragma once

#include "io/window.hpp"
#include "scene/scene.hpp"
#include "render/pass.hpp"

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
