#include "pass_light.hpp"

Pass_Light::Pass_Light(const std::string &pass_name, std::shared_ptr<Scene> scene_ptr, bool is_comp)
: Pass(pass_name, scene_ptr, is_comp) {}

void Pass_Light::render_pass()
{
    if (scene->scene_config->render_light) {
        setup_framebuffer_with_copy(*scene->screen_width, *scene->screen_height, scene->present_fbo);

        for (auto& light : scene->point_light_list) {
            auto position = light->position;
            auto color = light->color;
            auto radius = light->radius;

            auto &material = scene->model_cube->materials[0];
            auto &mesh = scene->model_cube->meshes[0];

            glm::mat4 model_matrix = glm::identity<glm::mat4x4>();
            model_matrix = glm::translate(model_matrix, position);
            model_matrix = glm::scale(model_matrix, glm::vec3(radius * 0.5f));

            shader->setMat4("model", model_matrix);
            shader->setMat4("view", scene->camera->get_view_matrix());
            shader->setMat4("projection", scene->camera->get_projection_matrix());

            shader->setVec3("lightColor", color);

            render_cube(*mesh, *material);
        }

        for (auto& light : scene->spot_light_list) {
            auto position = light->position;
            auto color = light->color;
            auto radius = light->radius;

            auto &material = scene->model_cube->materials[0];
            auto &mesh = scene->model_cube->meshes[0];

            glm::mat4 model_matrix = glm::identity<glm::mat4x4>();
            auto rotation_radian = glm::radians(light->direction);
            model_matrix = glm::translate(model_matrix, position);
            model_matrix = glm::scale(model_matrix, glm::vec3(radius * 0.5f));
            model_matrix = glm::rotate(model_matrix, rotation_radian.x, glm::vec3(-1.0f, 0.0f, 0.0f));
            model_matrix = glm::rotate(model_matrix, rotation_radian.y, glm::vec3(0.0f, 0.0f, -1.0f));

            shader->setMat4("model", model_matrix);
            shader->setMat4("view", scene->camera->get_view_matrix());
            shader->setMat4("projection", scene->camera->get_projection_matrix());

            shader->setVec3("lightColor", color);

            render_cube(*mesh, *material);
        }
    }
}
