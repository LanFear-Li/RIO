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
    void reset();

    void setup_framebuffer();

    void render(Mesh &mesh, Material &material);
    void render_cubemap(Mesh &mesh, Material &material);

    std::string name;
    std::unique_ptr<Shader> shader;

    unsigned int fbo;
    unsigned int rbo;
    std::unique_ptr<Texture> output;

    bool state_depth_test{true};
    Depth_Func depth_func{Depth_Func::less};
};
