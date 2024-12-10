#include "pass_compute_SAT.hpp"

#include "platform/api-function.hpp"

Pass_Compute_SAT::Pass_Compute_SAT(const std::string &pass_name, std::shared_ptr<Scene> scene_ptr, bool is_comp)
: Pass(pass_name, scene_ptr, is_comp) {}

void Pass_Compute_SAT::render_pass()
{
    if (scene->scene_config->render_shadow &&
    (scene->scene_config->shadow_method == Shadow_Method::VSM || scene->scene_config->shadow_method == Shadow_Method::VSSM)) {
        shader->setInt("input_image", 0);
        shader->setInt("output_image", 1);

        for (int i = 0; i < scene->directional_light_list.size(); i++) {
            setup_framebuffer_comp_SAT(1024, 1024);
            render_comp_SAT(scene->directional_shadow_map_list[i]->get_id());

            scene->directional_SAT_map_list[i] = std::move(SAT_map[1]);
        }

        for (int i = 0; i < scene->spot_light_list.size(); i++) {
            setup_framebuffer_comp_SAT(1024, 1024);
            render_comp_SAT(scene->spot_shadow_map_list[i]->get_id());

            scene->spot_SAT_map_list[i] = std::move(SAT_map[1]);
        }
    }
}

void Pass_Compute_SAT::setup_framebuffer_comp_SAT(int width, int height)
{
    frame_buffer = std::make_shared<Frame_Buffer>();

    SAT_map[0] = create_texture_RG(width, height);
    frame_buffer->attach_texture(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, SAT_map[0]->get_id());
    Api_Function::clear_color_and_depth();
    SAT_map[1] = create_texture_RG(width, height);
    frame_buffer->attach_texture(GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, SAT_map[1]->get_id());
    Api_Function::clear_color_and_depth();

    frame_buffer->unbind();

    buffer_width = width;
    buffer_height = height;
}

void Pass_Compute_SAT::render_comp_SAT(GLuint shadow_map)
{
    Api_Function::set_viewport(buffer_width, buffer_height);

    glBindImageTexture(0, shadow_map, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
    glBindImageTexture(1, SAT_map[0]->get_id(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F);
    glDispatchCompute(buffer_width, 1, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    glBindImageTexture(0, SAT_map[0]->get_id(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
    glBindImageTexture(1, SAT_map[1]->get_id(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F);
    glDispatchCompute(buffer_width, 1, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}
