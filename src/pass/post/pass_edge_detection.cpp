#include "pass_edge_detection.hpp"

Pass_Edge_Detection::Pass_Edge_Detection(const std::string &pass_name, std::shared_ptr<Scene> scene_ptr, bool is_comp)
: Pass(pass_name, scene_ptr, is_comp) {}

void Pass_Edge_Detection::render_pass()
{
    setup_framebuffer(*scene->screen_width, *scene->screen_height, Texture_Type::TEXTURE_2D);

    auto &mesh = scene->model_quad->meshes[0];
    render_smaa(*mesh);

    tex_edge = std::move(output);
}

void Pass_Edge_Detection::render_smaa(const Mesh &mesh)
{
    shader_reset();
    shader->setVec4("SMAA_RT_METRICS", 1.0f / buffer_width, 1.0f / buffer_height, buffer_width, buffer_height);

    frame_buffer->bind_texture(GL_TEXTURE_2D, shader->ID, scene->shade_color->get_id(), "colorTex");

    render_quad(mesh);
}
