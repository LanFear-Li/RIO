#include "pass_skybox.hpp"

Pass_Skybox::Pass_Skybox(const std::string &pass_name, std::shared_ptr<Scene> scene_ptr, bool is_comp)
: Pass(pass_name, scene_ptr, is_comp) {}

void Pass_Skybox::render_pass()
{
    if (scene->scene_config->show_skybox) {
        setup_framebuffer_with_copy(*(scene->screen_width), *(scene->screen_height), scene->present_fbo);

        auto &material = scene->model_skybox->materials[0];
        auto &mesh = scene->model_skybox->meshes[0];

        glm::mat4 model_matrix = glm::identity<glm::mat4x4>();
        shader->setMat4("model", model_matrix);
        shader->setMat4("view", scene->camera->get_view_matrix());
        shader->setMat4("projection", scene->camera->get_projection_matrix());

        render_cube_skybox(*mesh, *material);
    }
}

void Pass_Skybox::render_cube_skybox(const Mesh &mesh, const Material &material)
{
    // Specially for pass_skybox.
    if (material.skybox_map != nullptr) {
        frame_buffer->bind_texture(GL_TEXTURE_CUBE_MAP, shader->ID, material.skybox_map->get_id(), "environment_map");
    }

    render_cube(mesh, material);
}
