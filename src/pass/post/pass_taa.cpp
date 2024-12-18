#include "pass_taa.hpp"

Pass_TAA::Pass_TAA(const std::string &pass_name, std::shared_ptr<Scene> scene_ptr, bool is_comp)
: Pass(pass_name, scene_ptr, is_comp) {}

void Pass_TAA::render_pass()
{
    setup_framebuffer(*scene->screen_width, *scene->screen_height, Texture_Type::TEXTURE_2D);

    auto &mesh = scene->model_quad->meshes[0];
    render_taa(*mesh);

    // Blit the TAA framebuffer to the shaded framebuffer.
    Pass::blit_framebuffer(frame_buffer->get_fbo_id(), scene->shade_fbo->get_fbo_id(), buffer_width, buffer_height);

    color_previous = std::move(output);
}

void Pass_TAA::render_taa(const Mesh &mesh)
{
    shader_reset();

    shader->setInt("frame_count", scene->frame_count);
    shader->setInt("buffer_width", buffer_width);
    shader->setInt("buffer_height", buffer_height);

    frame_buffer->bind_texture(GL_TEXTURE_2D, shader->ID, scene->shade_color->get_id(), "color_current");
    frame_buffer->bind_texture(GL_TEXTURE_2D, shader->ID, scene->shade_depth->get_id(), "depth_current");

    frame_buffer->bind_texture(GL_TEXTURE_2D, shader->ID, color_previous ? color_previous->get_id() : 0, "color_previous");
    frame_buffer->bind_texture(GL_TEXTURE_2D, shader->ID, scene->velocity_tex->get_id(), "velocity_tex");

    render_quad(mesh);
}
