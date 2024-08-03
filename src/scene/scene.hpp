#pragma once

#include <vector>

#include "render/pass.hpp"
#include "camera.hpp"

#include "model.hpp"
#include "scene/light.hpp"

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
