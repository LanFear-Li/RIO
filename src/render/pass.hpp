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

    void setup_framebuffer(int width, int height, Texture_Type type, bool mipmap=false);
    void setup_framebuffer_default(int width, int height);

    int buffer_width;
    int buffer_height;

    void render(Mesh &mesh, Material &material, IBL_Data &ibl_data);
    void render_cubemap(Mesh &mesh, Texture &texture);
    void render_cubemap_mipmap(Mesh &mesh, Texture &texture);
    void render_quad(Mesh &mesh);

    std::string name;
    std::unique_ptr<Shader> shader;

    unsigned int fbo;
    unsigned int rbo;
    std::unique_ptr<Texture> output;

    bool state_depth_test{true};
    Depth_Func depth_func{Depth_Func::less};
};
