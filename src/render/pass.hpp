#pragma once

#include "scene/model.hpp"
#include "render/shader.hpp"

struct Pass
{
public:
    Pass() = default;

    void shaderInit();
    void prepare();
    void active();
    void render(Model &model);

    Shader shader;
};
