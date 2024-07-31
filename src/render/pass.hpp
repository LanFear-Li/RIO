#pragma once

#include "model.hpp"
#include "shader.hpp"

struct Pass
{
public:
    Pass() = default;

    void shaderInit();
    void active();
    void render(Model &model);

    Shader shader;
};
