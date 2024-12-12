#include "pass_fxaa.hpp"

Pass_FXAA::Pass_FXAA(const std::string &pass_name, std::shared_ptr<Scene> scene_ptr, bool is_comp)
: Pass(pass_name, scene_ptr, is_comp) {}

void Pass_FXAA::render_pass()
{
    setup_framebuffer(*scene->screen_width, *scene->screen_height, Texture_Type::TEXTURE_2D);

    auto &mesh = scene->model_quad->meshes[0];
    render_quad_post(*mesh, scene->shade_color, "input_fxaa");

    scene->shade_color = std::move(output);

    scene->shade_fbo->bind();
    scene->shade_fbo->attach_texture(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, scene->shade_color->get_id());
    scene->shade_fbo->unbind();
}
