#pragma once

#include "scene/camera.hpp"
#include "scene/model.hpp"
#include "scene/light.hpp"
#include "render/pass.hpp"

#include <vector>

struct Scene
{
public:
    Scene();

    void update();
    void render(Pass &render_pass);

    Camera camera;
    std::vector<Model *> model_list;
    std::vector<PointLight *> point_light_list;
};
