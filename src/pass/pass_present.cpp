#include "pass_present.hpp"

#include "platform/api-function.hpp"

Pass_Present::Pass_Present(const std::string &pass_name, std::shared_ptr<Scene> scene_ptr, bool is_comp, bool no_shader)
: Pass(pass_name, scene_ptr, is_comp, no_shader) {}

void Pass_Present::render_pass()
{
    setup_framebuffer_present(*(scene->screen_width), *(scene->screen_height));
    frame_buffer->bind();
    Api_Function::clear_color_and_depth();
    frame_buffer->unbind();

    // Blit the shaded framebuffer to the present framebuffer (default screen).
    blit_framebuffer(scene->shade_fbo->get_fbo_id(), frame_buffer->get_fbo_id(), buffer_width, buffer_height);
}

void Pass_Present::setup_framebuffer_present(int width, int height)
{
    frame_buffer = std::make_shared<Frame_Buffer>(0);

    buffer_width = width;
    buffer_height = height;
}
