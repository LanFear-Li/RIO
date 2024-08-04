#pragma once

#include "scene/model.hpp"
#include "render/shader.hpp"

#include <memory>

struct Pass
{
public:
    Pass() = delete;
    Pass(std::string shader_name);

    void prepare();
    void active();
    void render(Model &model);

    std::unique_ptr<Shader> shader;
};
