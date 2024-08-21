#pragma once

#include "scene/camera.hpp"
#include "scene/model.hpp"
#include "scene/config.hpp"
#include "scene/light.hpp"
#include "render/pass.hpp"

#include <vector>
#include <memory>

struct Scene
{
public:
    Scene() = delete;
    Scene(std::string scene_name);
    void prepare_scene(std::string scene_name);
    void update_model(std::string model_name);

    void update();
    void render(Pass &render_pass);

    std::unique_ptr<Camera> camera;
    std::vector<std::unique_ptr<PointLight>> point_light_list;

    std::vector<std::unique_ptr<Model>> model_list;
    std::vector<const char *> candidate_model_list;

    // Specially for skybox.
    std::unique_ptr<Model> model_ibl;

    // Specially for light.
    std::unique_ptr<Model> model_light;

    std::shared_ptr<Panel_Config> scene_config;
};
