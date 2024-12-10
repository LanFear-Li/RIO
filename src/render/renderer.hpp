#pragma once

#include "io/window.hpp"
#include "scene/scene.hpp"
#include "io/panel.hpp"
#include "render/pass.hpp"

#include <memory>

class Renderer final
{
public:
    Renderer() = delete;
    Renderer(const std::string &scene_name);

    void run() const;

private:
    std::unique_ptr<Window> window;
    std::shared_ptr<Scene> scene;
    std::unique_ptr<Panel> panel;

    // Pass for lighting / shadowing.
    std::unique_ptr<Pass> pass_shadow;

    // Pass for shading.
    std::unique_ptr<Pass> pass_shade;

    // Pass for environment lighting.
    std::unique_ptr<Pass> pass_ibl;

    // Pass for post processing and anti-aliasing.
    std::unique_ptr<Pass> pass_post_process;
};
