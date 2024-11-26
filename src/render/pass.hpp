#pragma once

#include "scene/model.hpp"
#include "platform/shader.hpp"
#include "platform/frame-buffer.hpp"
#include "platform/render-buffer.hpp"

#include <memory>

enum struct Depth_Func
{
    less,
    less_equal
};

class Pass final
{
public:
    Pass() = delete;
    Pass(const std::string &pass_name);

    void prepare() const;
    void shader_reset() const;
    void clear_depth() const;

    void setup_framebuffer(int width, int height, Texture_Type type, bool mipmap=false);
    void setup_framebuffer(int width, int height, std::shared_ptr<Frame_Buffer> buffer);
    void setup_framebuffer_depth(int width, int height);

    void render(const Mesh &mesh, const Material &material, const IBL_Data &ibl_data);
    void render_others(const Mesh &mesh, const Material &material);
    void render_depth(const Mesh &mesh);

    void render_cubemap(const Mesh &mesh, const Texture &texture);
    void render_cubemap_mipmap(const Mesh &mesh, const Texture &texture);
    void render_quad(const Mesh &mesh);

    std::string name;
    std::unique_ptr<Shader> shader;

    std::unique_ptr<Texture> output;
    std::shared_ptr<Frame_Buffer> frame_buffer;

    bool state_depth_test{true};
    Depth_Func depth_func{Depth_Func::less};

private:
    int buffer_width;
    int buffer_height;
};
