#pragma once

#include "model.hpp"
#include "shader.hpp"

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
