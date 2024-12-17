#include "pass_grading.hpp"

#include "platform/api-function.hpp"

Pass_Grading::Pass_Grading(const std::string &pass_name, std::shared_ptr<Scene> scene_ptr, bool is_comp)
: Pass(pass_name, scene_ptr, is_comp) {}

void Pass_Grading::render_pass()
{
    setup_framebuffer(*scene->screen_width, *scene->screen_height, Texture_Type::TEXTURE_2D);

    auto &mesh = scene->model_quad->meshes[0];
    render_quad_post(*mesh, scene->shade_color, "input_grading");

    // Blit the grading framebuffer to the shaded framebuffer.
    Pass::blit_framebuffer(frame_buffer->get_fbo_id(), scene->shade_fbo->get_fbo_id(), buffer_width, buffer_height);
}
