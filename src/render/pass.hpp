#pragma once

#include "scene/model.hpp"
#include "render/shader.hpp"

#include <memory>

enum struct Depth_Func
{
    less,
    less_equal
};

struct Pass
{
public:
    Pass() = delete;
    Pass(std::string pass_name);

    void prepare();
    void active();
    void render(Mesh &mesh, Material &material);

    std::unique_ptr<Shader> shader;
    std::string name;

    bool state_depth_test{true};
    Depth_Func depth_func{Depth_Func::less};
};
