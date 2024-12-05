#pragma once

#include "scene/scene.hpp"
#include "platform/shader.hpp"
#include "platform/frame-buffer.hpp"
#include "platform/render-buffer.hpp"
#include "platform/time-query.hpp"

enum struct Depth_Func
{
    less,
    less_equal
};

class Pass
{
public:
    Pass() = delete;
    Pass(const std::string &pass_name, std::shared_ptr<Scene> scene_ptr, bool is_comp=false);

    void prepare() const;
    void shader_reset() const;
    void clear_depth() const;

    void render();
    virtual void render_pass() {};

    void setup_framebuffer(int width, int height, Texture_Type type, bool mipmap=false);
    void setup_framebuffer(int width, int height, std::shared_ptr<Frame_Buffer> buffer);
    void setup_framebuffer_depth(int width, int height, bool shadow_vsm);
    void setup_framebuffer_comp_SAT(int width, int height);

    void render_color(const Mesh &mesh, const Material &material, const IBL_Data &ibl_data);
    void render_others(const Mesh &mesh, const Material &material);
    void render_depth(const Mesh &mesh);
    void render_comp_SAT(GLuint shadow_map);

    void render_cubemap(const Mesh &mesh, const Texture &texture);
    void render_cubemap_mipmap(const Mesh &mesh, const Texture &texture);
    void render_quad(const Mesh &mesh);

    std::string name;
    std::unique_ptr<Shader> shader;

    std::unique_ptr<Texture> output;
    std::shared_ptr<Frame_Buffer> frame_buffer;
    std::unique_ptr<Render_Buffer> render_buffer;

    std::unique_ptr<Texture> SAT_map[2];

    bool state_depth_test{true};
    Depth_Func depth_func{Depth_Func::less};

    std::shared_ptr<Pass_Profile_Info> profile_info;
    std::unique_ptr<Time_Query> time_query;

    std::shared_ptr<Scene> scene;

private:
    int buffer_width;
    int buffer_height;
};
