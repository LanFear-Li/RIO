#pragma once

#include "io/window.hpp"
#include "scene/scene.hpp"
#include "scene/panel.hpp"
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
    std::shared_ptr<Scene> scene;
    std::unique_ptr<Panel> panel;

    std::unique_ptr<Pass> pass_shade;
    std::unique_ptr<Pass> pass_light;

    // Convert Equirectangular Map to CubeMap.
    std::unique_ptr<Pass> pass_rect_to_cube;
    std::unique_ptr<Pass> pass_skybox;

    // Irradiance Map for diffuse IBL.
    std::unique_ptr<Pass> pass_ibl_irradiance;
    // Pre-Filtered Environment Map for specular IBL.
    std::unique_ptr<Pass> pass_ibl_prefiltered_map;
    // Precomputed Environment BRDF for specular IBL.
    std::unique_ptr<Pass> pass_ibl_precomputed_brdf;
};
