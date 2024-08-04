#pragma once

#include "scene/camera.hpp"
#include "scene/model.hpp"
#include "scene/light.hpp"
#include "render/pass.hpp"

#include <vector>
#include <memory>

struct Scene
{
public:
    Scene() = delete;
    Scene(std::string scene_name);

    void update();
    void render(Pass &render_pass);

    std::unique_ptr<Camera> camera;
    std::vector<Model *> model_list;
    std::vector<PointLight *> point_light_list;
};
