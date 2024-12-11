#include "pass_shade.hpp"

#include "pass_skybox.hpp"
#include "pass_light.hpp"

#include "platform/api-function.hpp"

Pass_Shade::Pass_Shade(const std::string &pass_name, std::shared_ptr<Scene> scene_ptr, bool is_comp)
: Pass(pass_name, scene_ptr, is_comp)
{
    pass_skybox = std::make_unique<Pass_Skybox>("skybox", scene);
    pass_skybox->depth_func = Depth_Func::less_equal;
    pass_light = std::make_unique<Pass_Light>("light", scene);
    pass_light->depth_func = Depth_Func::less_equal;
}

void Pass_Shade::setup_framebuffer_shade(int width, int height)
{
    scene->shade_fbo = std::make_shared<Frame_Buffer>();
    scene->shade_color = create_texture(Texture_Type::TEXTURE_2D, width, height);
    scene->shade_depth = create_texture(Texture_Type::TEXTURE_2D_DEPTH, width, height);

    scene->shade_fbo->attach_texture(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, scene->shade_color->get_id());
    scene->shade_fbo->attach_texture(GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, scene->shade_depth->get_id());

    Api_Function::clear_color_and_depth();
    scene->shade_fbo->unbind();

    frame_buffer = scene->shade_fbo;
    buffer_width = width;
    buffer_height = height;
}

void Pass_Shade::render_pass()
{
    setup_framebuffer_shade(*scene->screen_width, *scene->screen_height);

    bool shadow_vsm = (scene->scene_config->shadow_method == Shadow_Method::VSM ||
    scene->scene_config->shadow_method == Shadow_Method::VSSM);

    for (auto &model : scene->model_list) {
        for (auto &mesh : model->meshes) {
            shader_reset();

            auto &material = model->materials[mesh->material_index];

            glm::mat4 model_matrix = glm::identity<glm::mat4x4>();
            auto rotation_radian = glm::radians(model->rotation);
            model_matrix = glm::translate(model_matrix, model->position);
            model_matrix = glm::scale(model_matrix, glm::vec3(model->scaling));
            model_matrix = glm::rotate(model_matrix, rotation_radian.x, glm::vec3(1.0f, 0.0f, 0.0f));
            model_matrix = glm::rotate(model_matrix, rotation_radian.y, glm::vec3(0.0f, 1.0f, 0.0f));
            model_matrix = glm::rotate(model_matrix, rotation_radian.z, glm::vec3(0.0f, 0.0f, 1.0f));

            shader->setMat4("model", model_matrix);
            shader->setMat4("view", scene->camera->get_view_matrix());
            shader->setMat4("projection", scene->camera->get_projection_matrix());
            shader->setVec3("eyePos", scene->camera->cam_position);
            shader->setInt("shading_method", scene->scene_config->shading_method);
            shader->setVec3("ambient_color", scene->scene_config->ambient_color);

            shader->setBool("render_shadow", scene->scene_config->render_shadow);
            shader->setInt("shadow_method", scene->scene_config->shadow_method);

            int point_light_num = scene->point_light_list.size();
            for (int i = 0; i < point_light_num; i++) {
                auto &light = scene->point_light_list[i];
                std::string light_idx = "point_light[" + std::to_string(i) + "].";

                shader->setVec3(light_idx + "position", light->position);
                shader->setVec3(light_idx + "color", light->color);
                shader->setFloat(light_idx + "radius", light->radius);
                shader->setFloat(light_idx + "intensity", light->intensity);
            }

            int directional_light_num = scene->directional_light_list.size();
            for (int i = 0; i < directional_light_num; i++) {
                auto &light = scene->directional_light_list[i];
                std::string light_idx = "directional_light[" + std::to_string(i) + "].";

                glm::vec3 direction = euler_to_direction(light->direction);

                shader->setVec3(light_idx + "direction", direction);
                shader->setVec3(light_idx + "color", light->color);
                shader->setFloat(light_idx + "intensity", light->intensity);
            }

            for (int i = 0; i < MAX_LIGHT_NUM; i++) {
                auto shadow_map_name = "directional_shadow_map[" + std::to_string(i) + "]";
                auto SAT_map_name = "directional_SAT_map[" + std::to_string(i) + "]";

                if (scene->scene_config->render_shadow && i < directional_light_num) {
                    shader->setMat4("directional_light_matrix[" + std::to_string(i) + "]", scene->directional_light_matrix_list[i]);

                    auto tex_id = shadow_vsm ? scene->directional_SAT_map_list[i]->get_id() : 0;
                    frame_buffer->bind_texture(GL_TEXTURE_2D, shader->ID, scene->directional_shadow_map_list[i]->get_id(), shadow_map_name.c_str());
                    frame_buffer->bind_texture(GL_TEXTURE_2D, shader->ID, tex_id, SAT_map_name.c_str());
                } else {
                    frame_buffer->bind_texture(GL_TEXTURE_2D, shader->ID, 0, shadow_map_name.c_str());
                    frame_buffer->bind_texture(GL_TEXTURE_2D, shader->ID, 0, SAT_map_name.c_str());
                }
            }

            int spot_light_num = scene->spot_light_list.size();
            for (int i = 0; i < spot_light_num; i++) {
                auto &light = scene->spot_light_list[i];

                std::string light_idx = "spot_light[" + std::to_string(i) + "].";
                glm::vec3 direction = euler_to_direction(light->direction);

                shader->setVec3(light_idx + "position", light->position);
                shader->setVec3(light_idx + "direction", direction);
                shader->setFloat(light_idx + "cutoff", glm::cos(glm::radians(light->cutoff)));
                shader->setFloat(light_idx + "outer_cutoff", glm::cos(glm::radians(light->outer_cutoff)));
                shader->setVec3(light_idx + "color", light->color);
                shader->setFloat(light_idx + "radius", light->radius);
                shader->setFloat(light_idx + "intensity", light->intensity);
            }

            for (int i = 0; i < MAX_LIGHT_NUM; i++) {
                auto shadow_map_name = "spot_shadow_map[" + std::to_string(i) + "]";
                auto SAT_map_name = "spot_SAT_map[" + std::to_string(i) + "]";

                if (scene->scene_config->render_shadow && i < spot_light_num) {
                    shader->setMat4("spot_light_matrix[" + std::to_string(i) + "]", scene->spot_light_matrix_list[i]);

                    auto tex_id = shadow_vsm ? scene->spot_SAT_map_list[i]->get_id() : 0;
                    frame_buffer->bind_texture(GL_TEXTURE_2D, shader->ID, scene->spot_shadow_map_list[i]->get_id(), shadow_map_name.c_str());
                    frame_buffer->bind_texture(GL_TEXTURE_2D, shader->ID, tex_id, SAT_map_name.c_str());
                } else {
                    frame_buffer->bind_texture(GL_TEXTURE_2D, shader->ID, 0, shadow_map_name.c_str());
                    frame_buffer->bind_texture(GL_TEXTURE_2D, shader->ID, 0, SAT_map_name.c_str());
                }
            }

            shader->setInt("point_light_num", point_light_num);
            shader->setInt("directional_light_num", directional_light_num);
            shader->setInt("spot_light_num", spot_light_num);

            shader->setBool("use_ibl_data", scene->scene_config->enable_ibl && scene->ibl_generated);

            render_color(*mesh, *material, *scene->ibl_data);
        }
    }

    pass_skybox->render();
    pass_light->render();
}
