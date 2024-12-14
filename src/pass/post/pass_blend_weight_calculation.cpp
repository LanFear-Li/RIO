#include "pass_blend_weight_calculation.hpp"

Pass_Blend_Weight_Calculation::Pass_Blend_Weight_Calculation(const std::string &pass_name, std::shared_ptr<Scene> scene_ptr, bool is_comp)
: Pass(pass_name, scene_ptr, is_comp)
{
    tex_area = std::move(Model::construct_texture("smaa/area.png"));
    tex_search = std::move(Model::construct_texture("smaa/search.png"));
}

void Pass_Blend_Weight_Calculation::render_pass()
{
    setup_framebuffer(*scene->screen_width, *scene->screen_height, Texture_Type::TEXTURE_2D);

    auto &mesh = scene->model_quad->meshes[0];
    render_smaa(*mesh);

    tex_blend = std::move(output);
}

void Pass_Blend_Weight_Calculation::render_smaa(const Mesh &mesh)
{
    shader->setVec4("SMAA_RT_METRICS", 1.0f / buffer_width, 1.0f / buffer_height, buffer_width, buffer_height);

    frame_buffer->bind_texture(GL_TEXTURE_2D, shader->ID, scene->shade_color->get_id(), "colorTex");
    frame_buffer->bind_texture(GL_TEXTURE_2D, shader->ID, tex_area->get_id(), "areaTex");
    frame_buffer->bind_texture(GL_TEXTURE_2D, shader->ID, tex_search->get_id(), "searchTex");
    frame_buffer->bind_texture(GL_TEXTURE_2D, shader->ID, tex_edge->get_id(), "edgeTex");

    render_quad(mesh);
}
