#pragma once

#include "io/window.hpp"
#include "scene/scene.hpp"
#include "render/pass.hpp"

#include <memory>

class Renderer
{
public:
    Renderer() = delete;
    Renderer(std::string scene_name);
    void run();

private:
    std::unique_ptr<Window> window;
    std::unique_ptr<Scene> scene;

    std::unique_ptr<Pass> pass_shade;
    std::unique_ptr<Pass> pass_ibl;
};
