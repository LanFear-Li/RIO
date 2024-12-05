#include "pass_shadow.hpp"

Pass_Shadow::Pass_Shadow(const std::string &pass_name, std::shared_ptr<Scene> scene_ptr, bool is_comp)
: Pass(pass_name, scene_ptr, is_comp) {}

void Pass_Shadow::render_pass()
{
    if (scene->scene_config->render_shadow) {
        bool shadow_vsm = (scene->scene_config->shadow_method == Shadow_Method::VSM ||
        scene->scene_config->shadow_method == Shadow_Method::VSSM);

        shader->setBool("use_vsm", shadow_vsm);

        for (int i = 0; i < scene->directional_light_list.size(); i++) {
            setup_framebuffer_depth(1024, 1024, shadow_vsm);
            clear_depth();

            auto &light = scene->directional_light_list[i];

            float box_size = 5.0f;
            glm::vec3 bbox_min = glm::vec3(-box_size, -box_size, -box_size);
            glm::vec3 bbox_max = glm::vec3(box_size, box_size, box_size);
            glm::vec3 bbox_center = (bbox_min + bbox_max) / 2.0f;
            glm::vec3 half_extent = (bbox_max - bbox_min) / 2.0f;
            float max_extent = glm::max(half_extent.x, glm::max(half_extent.y, half_extent.z));

            auto light_direction = euler_to_direction(light->direction);
            auto light_position = bbox_center - light_direction * max_extent;

            auto view = glm::lookAt(light_position, bbox_center, glm::vec3(0.0, 1.0, 0.0));
            auto projection = glm::ortho(bbox_min.x, bbox_max.x, bbox_min.x, bbox_max.x, 0.1f, 10.0f);

            shader->setMat4("view", view);
            shader->setMat4("projection", projection);
            scene->directional_light_matrix_list[i] = projection * view;

            for (auto &model : scene->model_list) {
                for (auto &mesh : model->meshes) {
                    glm::mat4 model_matrix = glm::identity<glm::mat4x4>();
                    auto rotation_radian = glm::radians(model->rotation);
                    model_matrix = glm::translate(model_matrix, model->position);
                    model_matrix = glm::scale(model_matrix, glm::vec3(model->scaling));
                    model_matrix = glm::rotate(model_matrix, rotation_radian.x, glm::vec3(1.0f, 0.0f, 0.0f));
                    model_matrix = glm::rotate(model_matrix, rotation_radian.y, glm::vec3(0.0f, 1.0f, 0.0f));
                    model_matrix = glm::rotate(model_matrix, rotation_radian.z, glm::vec3(0.0f, 0.0f, 1.0f));

                    shader->setMat4("model", model_matrix);

                    render_depth(*mesh);
                }
            }

            scene->directional_shadow_map_list[i] = std::move(output);
        }

        for (int i = 0; i < scene->spot_light_list.size(); i++) {
            setup_framebuffer_depth(1024, 1024, shadow_vsm);
            clear_depth();

            auto &light = scene->spot_light_list[i];

            auto light_direction = euler_to_direction(light->direction);
            auto light_position = light->position;
            glm::vec3 center = light_position + light_direction;

            float box_size = 5.0f;
            auto view = glm::lookAt(light_position, center, glm::vec3(0.0, 1.0, 0.0));
            auto projection = glm::ortho(-box_size, box_size, -box_size, box_size, 0.1f, 10.0f);

            shader->setMat4("view", view);
            shader->setMat4("projection", projection);
            scene->spot_light_matrix_list[i] = projection * view;

            for (auto &model : scene->model_list) {
                for (auto &mesh : model->meshes) {
                    glm::mat4 model_matrix = glm::identity<glm::mat4x4>();
                    auto rotation_radian = glm::radians(model->rotation);
                    model_matrix = glm::translate(model_matrix, model->position);
                    model_matrix = glm::scale(model_matrix, glm::vec3(model->scaling));
                    model_matrix = glm::rotate(model_matrix, rotation_radian.x, glm::vec3(1.0f, 0.0f, 0.0f));
                    model_matrix = glm::rotate(model_matrix, rotation_radian.y, glm::vec3(0.0f, 1.0f, 0.0f));
                    model_matrix = glm::rotate(model_matrix, rotation_radian.z, glm::vec3(0.0f, 0.0f, 1.0f));

                    shader->setMat4("model", model_matrix);

                    render_depth(*mesh);
                }
            }

            scene->spot_shadow_map_list[i] = std::move(output);
        }
    }
}
