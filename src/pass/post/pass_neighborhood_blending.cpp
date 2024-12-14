#include "pass_neighborhood_blending.hpp"

Pass_Neighborhood_Blending::Pass_Neighborhood_Blending(const std::string &pass_name, std::shared_ptr<Scene> scene_ptr, bool is_comp)
: Pass(pass_name, scene_ptr, is_comp) {}

void Pass_Neighborhood_Blending::render_pass()
{
    setup_framebuffer(*scene->screen_width, *scene->screen_height, Texture_Type::TEXTURE_2D);

    auto &mesh = scene->model_quad->meshes[0];
    render_smaa(*mesh);

    scene->shade_color = std::move(output);
}

void Pass_Neighborhood_Blending::render_smaa(const Mesh &mesh)
{
    shader->setVec4("SMAA_RT_METRICS", 1.0f / buffer_width, 1.0f / buffer_height, buffer_width, buffer_height);

    frame_buffer->bind_texture(GL_TEXTURE_2D, shader->ID, tex_blend->get_id(), "blendTex");

    render_quad(mesh);
}
