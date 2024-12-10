#include "pass_rect_to_cube.hpp"

Pass_Rect_To_Cube::Pass_Rect_To_Cube(const std::string &pass_name, std::shared_ptr<Scene> scene_ptr, bool is_comp)
: Pass(pass_name, scene_ptr, is_comp) {}

void Pass_Rect_To_Cube::render_pass()
{
    if (scene->scene_config->show_skybox && !scene->cubemap_converted) {
        setup_framebuffer(1024, 1024, Texture_Type::TEXTURE_CUBE_MAP);

        auto &material = scene->model_skybox->materials[0];
        auto &mesh = scene->model_skybox->meshes[0];

        render_cubemap_rect(*mesh, *material->skybox_map);

        material->skybox_map = std::move(output);
        scene->ibl_data->environment_map = material->skybox_map;
        scene->cubemap_converted = true;
    }
}

void Pass_Rect_To_Cube::render_cubemap_rect(const Mesh &mesh, const Texture &texture)
{
    frame_buffer->bind_texture(GL_TEXTURE_2D, shader->ID, texture.get_id(), "equirectangular_map");

    render_cubemap(mesh, texture);
}
