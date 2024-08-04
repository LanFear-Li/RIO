#pragma once

#include "io/window.hpp"
#include "scene/scene.hpp"
#include "render/pass.hpp"

#include <memory>

// default settings
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 800;

const unsigned int SHADOW_WIDTH = 1024;
const unsigned int SHADOW_HEIGHT = 1024;

class Renderer
{
public:
    Renderer() = delete;
    Renderer(std::string scene_name);
    void run();

private:
    std::unique_ptr<Window> window;
    std::unique_ptr<Scene> scene;
    std::unique_ptr<Pass> pass;
};
