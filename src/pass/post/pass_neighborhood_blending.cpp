#include "pass_neighborhood_blending.hpp"

#include "platform/api-function.hpp"

Pass_Neighborhood_Blending::Pass_Neighborhood_Blending(const std::string &pass_name, std::shared_ptr<Scene> scene_ptr, bool is_comp)
: Pass(pass_name, scene_ptr, is_comp) {}

void Pass_Neighborhood_Blending::render_pass()
{
    setup_framebuffer(*scene->screen_width, *scene->screen_height, Texture_Type::TEXTURE_2D);

    auto &mesh = scene->model_quad->meshes[0];
    render_smaa(*mesh);

    // Blit the SMAA framebuffer to the shaded framebuffer.
    Pass::blit_framebuffer(frame_buffer->get_fbo_id(), scene->shade_fbo->get_fbo_id(), buffer_width, buffer_height);
}

void Pass_Neighborhood_Blending::render_smaa(const Mesh &mesh)
{
    shader_reset();
    shader->setVec4("SMAA_RT_METRICS", 1.0f / buffer_width, 1.0f / buffer_height, buffer_width, buffer_height);

    frame_buffer->bind_texture(GL_TEXTURE_2D, shader->ID, scene->shade_color->get_id(), "colorTex");
    frame_buffer->bind_texture(GL_TEXTURE_2D, shader->ID, tex_blend->get_id(), "blendTex");

    render_quad(mesh);
}
