#include "scene.hpp"

#include "io/filesystem.hpp"

#include <nlohmann/json.hpp>
#include <glm/gtx/euler_angles.hpp>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <filesystem>

namespace fs = std::filesystem;

glm::vec3 euler_to_direction(glm::vec3 euler) {
    float pitch = glm::radians(90.0f - euler.x);
    float yaw = glm::radians(euler.y);
    float roll = glm::radians(euler.z);

    // Euler (0,   0, 0) -> Points to +Y.
    // Euler (90,  0, 0) -> Points to -Z.
    // Euler (180, 0, 0) -> Points to -Y.
    glm::vec3 direction;
    direction.x = cos(pitch) * sin(yaw);
    direction.y = sin(pitch);
    direction.z = -cos(pitch) * cos(yaw);

    return glm::normalize(direction);
}

Scene::Scene(std::string scene_name)
{
    std::string file_path = FileSystem::getPath("runtime/assets/scenes/" + scene_name + ".json");
    auto scene_json = nlohmann::json::parse(std::ifstream(file_path), nullptr, false);
    if (scene_json.is_discarded()) {
        std::cout << "Initialize scene failed, check your scene config please." << std::endl;
        assert(0);
    }

    auto json_to_vec3 = [](nlohmann::json & json) -> glm::vec3 {
        return glm::vec3(json[0].get<float>(), json[1].get<float>(), json[2].get<float>());
    };

    // Load camera from scene.
    auto camera_json = scene_json["camera"];
    auto camera_pos = json_to_vec3(camera_json["position"]);

    camera = std::make_unique<Camera>(camera_pos);

    if (camera_json.contains("resolution")) {
        camera->cameraWidth = camera_json["resolution"][0].get<uint32_t>();
        camera->cameraHeight = camera_json["resolution"][1].get<uint32_t>();
    }

    if (camera_json.contains("draw-distance")) {
        camera->z_near = camera_json["draw-distance"][0].get<float>();
        camera->z_far = camera_json["draw-distance"][1].get<float>();
    }

    // Init scene config.
    screen_width = camera->cameraWidth;
    screen_height = camera->cameraHeight;
    scene_config = std::make_shared<Panel_Config>();
}

void Scene::prepare_scene(std::string scene_name)
{
    std::string file_path = FileSystem::getPath("runtime/assets/scenes/" + scene_name + ".json");
    auto scene_json = nlohmann::json::parse(std::ifstream(file_path), nullptr, false);
    if (scene_json.is_discarded()) {
        std::cout << "Initialize scene failed, check your scene config please." << std::endl;
        assert(0);
    }

    auto json_to_vec3 = [](nlohmann::json & json) -> glm::vec3 {
        return glm::vec3(json[0].get<float>(), json[1].get<float>(), json[2].get<float>());
    };

    // Load Model from scene.
    auto model_json = scene_json["models"];
    for (auto& [name, config] : model_json.items()) {
        std::string model_name = config["name"].get<std::string>();
        std::string model_path = get_model_path(model_name);

        auto model = std::make_unique<Model>(model_path);
        model->model_name = model_name;
        model->position = json_to_vec3(config["position"]);
        model->rotation = json_to_vec3(config["rotation"]);
        model->scaling = config["scaling"].get<float>();

        model_list.push_back(std::move(model));
    }

    // Load candidate model from assets/models.
    std::string candidate_model_path = FileSystem::getPath("runtime/assets/models/");
    for (const auto& entry : fs::directory_iterator(candidate_model_path)) {
        if (fs::is_directory(entry.path())) {
            std::string dirName = entry.path().filename().string();
            char* dirNameCStr = new char[dirName.length() + 1];
            std::strcpy(dirNameCStr, dirName.c_str());
            candidate_model_list.push_back(dirNameCStr);
        }
    }

    // Init IBL data.
    ibl_data = std::make_unique<IBL_Data>();
    ibl_data->environment_map = std::move(create_texture(Texture_Type::TEXTURE_CUBE_MAP, 500, 500));
    ibl_data->irrandiance_map = create_texture(Texture_Type::TEXTURE_CUBE_MAP, 500, 500);
    ibl_data->prefiltered_map = create_texture(Texture_Type::TEXTURE_CUBE_MAP, 500, 500);
    ibl_data->precomputed_brdf = create_texture(Texture_Type::TEXTURE_2D, 500, 500);

    // Load skybox from scene.
    if (scene_json.contains("skybox")) {
        auto skybox_json = scene_json["skybox"];
        std::string skybox_name = skybox_json["name"].get<std::string>();
        std::string skybox_path = "runtime/assets/skybox/" + skybox_name + "/";
        skybox_path = FileSystem::getPath(skybox_path);

        model_skybox = Model::constructSkybox(skybox_path, skybox_name);
        model_skybox->model_name = skybox_name;

        auto &skybox_map = model_skybox->materials[0]->skybox_map;
        if (skybox_map->get_type() == Texture_Type::TEXTURE_CUBE_MAP) {
            cubemap_converted = true;
            ibl_data->environment_map = skybox_map;
        }
    }

    if (scene_json.contains("show_skybox")) {
        scene_config->show_skybox = scene_json["show_skybox"].get<bool>();
    }

    if (scene_json.contains("enable_ibl")) {
        scene_config->enable_ibl = scene_json["enable_ibl"].get<bool>();
    }

    if (model_skybox) {
        scene_config->skybox_name = model_skybox->model_name;
    }

    // Load candidate skybox from assets/skybox.
    std::string candidate_skybox_path = FileSystem::getPath("runtime/assets/skybox/");
    for (const auto& entry : fs::directory_iterator(candidate_skybox_path)) {
        if (fs::is_directory(entry.path())) {
            std::string dirName = entry.path().filename().string();
            char* dirNameCStr = new char[dirName.length() + 1];
            std::strcpy(dirNameCStr, dirName.c_str());
            candidate_skybox_list.push_back(dirNameCStr);
        }
    }

    // Load light from scene.
    auto light_json = scene_json["lights"];
    for (auto& [name, config] : light_json.items()) {
        auto type = config["type"].get<std::string>();

        if (type == "point") {
            auto light = std::make_unique<PointLight>();

            light->light_name = name;
            light->position = json_to_vec3(config["position"]);
            light->color = json_to_vec3(config["color"]);
            light->radius = config["radius"].get<float>();
            light->intensity = config["intensity"].get<float>();

            point_light_list.push_back(std::move(light));
        } else if (type == "directional") {
            auto light = std::make_unique<DirectionalLight>();

            light->light_name = name;
            light->direction = json_to_vec3(config["direction"]);
            light->color = json_to_vec3(config["color"]);
            light->intensity = config["intensity"].get<float>();

            directional_light_list.push_back(std::move(light));
        } else if (type == "spot") {
            auto light = std::make_unique<SpotLight>();

            light->light_name = name;
            light->position = json_to_vec3(config["position"]);
            light->direction = json_to_vec3(config["direction"]);
            light->cutoff = config["cutoff"].get<float>();
            light->outer_cutoff = config["outer-cutoff"].get<float>();
            light->color = json_to_vec3(config["color"]);
            light->radius = config["radius"].get<float>();
            light->intensity = config["intensity"].get<float>();

            spot_light_list.push_back(std::move(light));
        }
    }

    // Initialize light data related to shadow.
    int directional_light_count = directional_light_list.size();
    directional_shadow_map_list.resize(directional_light_count);
    directional_light_matrix_list.resize(directional_light_count);

    int spot_light_count = spot_light_list.size();
    spot_shadow_map_list.resize(spot_light_count);
    spot_light_matrix_list.resize(spot_light_count);

    int point_light_count = point_light_list.size();
    point_shadow_map_list.resize(point_light_count);

    model_cube = Model::constructCube();
    model_quad = Model::constructQuad();

    // Load config from scene.
    scene_config->model_name = model_list.front()->model_name;

    if (scene_json.contains("shading_method")) {
        scene_config->shading_method = scene_json["shading_method"].get<Shading_Method>();
    }

    if (scene_json.contains("shadow_method")) {
        scene_config->shadow_method = scene_json["shadow_method"].get<Shadow_Method>();
    }
}

std::string Scene::get_model_path(std::string model_name)
{
    std::string model_path = "runtime/assets/models/" + model_name + "/" + model_name;
    model_path = FileSystem::getPath(model_path);

    std::string model_exts[] = {".obj", ".gltf", ".fbx", ".dae"};
    for (const auto& ext : model_exts) {
        std::string path = model_path + ext;

        if (std::filesystem::exists(path)) {
            return path;
        }
    }

    std::cout << "Initialize model failed, make sure your model exists." << std::endl;
    assert(0);
    return {};
}

void Scene::update_model(std::string model_name)
{
    model_list.clear();

    std::string model_path = get_model_path(model_name);
    auto model = std::make_unique<Model>(model_path);
    model->model_name = model_name;

    model_list.push_back(std::move(model));
}

void Scene::update_skybox(std::string skybox_name)
{
    std::string skybox_path = "runtime/assets/skybox/" + skybox_name + "/";
    skybox_path = FileSystem::getPath(skybox_path);

    model_skybox = Model::constructSkybox(skybox_path, skybox_name);
    model_skybox->model_name = skybox_name;

    if (model_skybox->materials[0]->skybox_map->get_type() == Texture_Type::TEXTURE_CUBE_MAP) {
        cubemap_converted = true;
    } else {
        cubemap_converted = false;
    }

    // Reset environment map from skybox map.
    ibl_data->environment_map = model_skybox->materials[0]->skybox_map;
    ibl_generated = false;
}

void Scene::render(Pass &render_pass)
{
    auto pass_name = render_pass.name;

    if (pass_name == "rect_to_cube" && scene_config->show_skybox && !cubemap_converted) {
        render_pass.prepare();
        render_pass.active();
        render_pass.reset();
        render_pass.setup_framebuffer(1024, 1024, Texture_Type::TEXTURE_CUBE_MAP);

        auto &material = model_skybox->materials[0];
        auto &mesh = model_skybox->meshes[0];
        auto &shader = render_pass.shader;

        render_pass.render_cubemap(*mesh, *material->skybox_map);

        material->skybox_map = std::move(render_pass.output);
        ibl_data->environment_map = material->skybox_map;
        cubemap_converted = true;
    }

    if (pass_name == "skybox" && scene_config->show_skybox) {
        render_pass.prepare();
        render_pass.active();
        render_pass.reset();
        render_pass.setup_framebuffer_default(screen_width, screen_height);

        auto &material = model_skybox->materials[0];
        auto &mesh = model_skybox->meshes[0];
        auto &shader = render_pass.shader;

        glm::mat4 model_matrix = glm::identity<glm::mat4x4>();
        shader->setMat4("model", model_matrix);
        shader->setMat4("view", camera->GetViewMatrix());
        shader->setMat4("projection", camera->GetProjectionMatrix());

        render_pass.render(*mesh, *material, *ibl_data);
    }

    if (pass_name == "ibl_irradiance" && scene_config->enable_ibl && !ibl_generated) {
        render_pass.prepare();
        render_pass.active();
        render_pass.reset();
        render_pass.setup_framebuffer(32, 32, Texture_Type::TEXTURE_CUBE_MAP);

        auto &mesh = model_cube->meshes[0];
        render_pass.render_cubemap(*mesh, *ibl_data->environment_map);

        ibl_data->irrandiance_map = std::move(render_pass.output);
    }

    if (pass_name == "ibl_prefiltered_map" && scene_config->enable_ibl && !ibl_generated) {
        render_pass.prepare();
        render_pass.active();
        render_pass.reset();
        render_pass.setup_framebuffer(128, 128, Texture_Type::TEXTURE_CUBE_MAP ,true);

        auto &mesh = model_cube->meshes[0];
        render_pass.render_cubemap_mipmap(*mesh, *ibl_data->environment_map);

        ibl_data->prefiltered_map = std::move(render_pass.output);
    }

    if (pass_name == "ibl_precomputed_brdf" && scene_config->enable_ibl && !ibl_generated) {
        render_pass.prepare();
        render_pass.active();
        render_pass.reset();
        render_pass.setup_framebuffer(512, 512, Texture_Type::TEXTURE_2D);

        auto &mesh = model_quad->meshes[0];
        render_pass.render_quad(*mesh);

        ibl_data->precomputed_brdf = std::move(render_pass.output);
        ibl_generated = true;
    }

    if (pass_name == "shadow" && scene_config->render_shadow) {
        auto &shader = render_pass.shader;

        for (int i = 0; i < directional_light_list.size(); i++) {
            render_pass.active();
            render_pass.setup_framebuffer_depth(1024, 1024);
            glBindFramebuffer(GL_FRAMEBUFFER, render_pass.fbo);
            glClear(GL_DEPTH_BUFFER_BIT);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            auto &light = directional_light_list[i];

            float box_size = 20.0f;
            glm::vec3 bbox_min = glm::vec3(-box_size, -box_size, -box_size);
            glm::vec3 bbox_max = glm::vec3(box_size, box_size, box_size);
            glm::vec3 bbox_center = (bbox_min + bbox_max) / 2.0f;
            glm::vec3 half_extent = (bbox_max - bbox_min) / 2.0f;
            float max_extent = glm::max(half_extent.x, glm::max(half_extent.y, half_extent.z));

            auto light_direction = euler_to_direction(light->direction);
            auto light_position = bbox_center - light_direction * max_extent;

            auto view = glm::lookAt(light_position, bbox_center, glm::vec3(0.0, 1.0, 0.0));
            auto projection = glm::ortho(bbox_min.x, bbox_max.x, bbox_min.x, bbox_max.x, 1.0f, box_size * 2.0f);

            shader->setMat4("view", view);
            shader->setMat4("projection", projection);
            directional_light_matrix_list[i] = projection * view;

            for (auto &model : model_list) {
                for (auto &mesh : model->meshes) {
                    glm::mat4 model_matrix = glm::identity<glm::mat4x4>();
                    auto rotation_radian = glm::radians(model->rotation);
                    model_matrix = glm::translate(model_matrix, model->position);
                    model_matrix = glm::scale(model_matrix, glm::vec3(model->scaling));
                    model_matrix = glm::rotate(model_matrix, rotation_radian.x, glm::vec3(1.0f, 0.0f, 0.0f));
                    model_matrix = glm::rotate(model_matrix, rotation_radian.y, glm::vec3(0.0f, 1.0f, 0.0f));
                    model_matrix = glm::rotate(model_matrix, rotation_radian.z, glm::vec3(0.0f, 0.0f, 1.0f));

                    shader->setMat4("model", model_matrix);

                    render_pass.render_depth(*mesh);
                }
            }

            directional_shadow_map_list[i] = std::move(render_pass.output);
        }

        for (int i = 0; i < spot_light_list.size(); i++) {
            render_pass.active();
            render_pass.setup_framebuffer_depth(1024, 1024);
            glBindFramebuffer(GL_FRAMEBUFFER, render_pass.fbo);
            glClear(GL_DEPTH_BUFFER_BIT);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            auto &light = spot_light_list[i];

            auto light_direction = euler_to_direction(light->direction);
            auto light_position = light->position;
            glm::vec3 center = light_position + light_direction;

            float box_size = 20.0f;
            auto view = glm::lookAt(light_position, center, glm::vec3(0.0, 1.0, 0.0));
            auto projection = glm::ortho(-box_size, box_size, -box_size, box_size, 0.1f, box_size * 2.0f);

            shader->setMat4("view", view);
            shader->setMat4("projection", projection);
            spot_light_matrix_list[i] = projection * view;

            for (auto &model : model_list) {
                for (auto &mesh : model->meshes) {
                    glm::mat4 model_matrix = glm::identity<glm::mat4x4>();
                    auto rotation_radian = glm::radians(model->rotation);
                    model_matrix = glm::translate(model_matrix, model->position);
                    model_matrix = glm::scale(model_matrix, glm::vec3(model->scaling));
                    model_matrix = glm::rotate(model_matrix, rotation_radian.x, glm::vec3(1.0f, 0.0f, 0.0f));
                    model_matrix = glm::rotate(model_matrix, rotation_radian.y, glm::vec3(0.0f, 1.0f, 0.0f));
                    model_matrix = glm::rotate(model_matrix, rotation_radian.z, glm::vec3(0.0f, 0.0f, 1.0f));

                    shader->setMat4("model", model_matrix);

                    render_pass.render_depth(*mesh);
                }
            }

            spot_shadow_map_list[i] = std::move(render_pass.output);
        }
    }

    if (pass_name == "shade") {
        for (auto &model : model_list) {
            for (auto &mesh : model->meshes) {
                render_pass.prepare();
                render_pass.active();
                render_pass.reset();
                render_pass.setup_framebuffer_default(screen_width, screen_height);

                auto &material = model->materials[mesh->materialIndex];
                auto &shader = render_pass.shader;

                glm::mat4 model_matrix = glm::identity<glm::mat4x4>();
                auto rotation_radian = glm::radians(model->rotation);
                model_matrix = glm::translate(model_matrix, model->position);
                model_matrix = glm::scale(model_matrix, glm::vec3(model->scaling));
                model_matrix = glm::rotate(model_matrix, rotation_radian.x, glm::vec3(1.0f, 0.0f, 0.0f));
                model_matrix = glm::rotate(model_matrix, rotation_radian.y, glm::vec3(0.0f, 1.0f, 0.0f));
                model_matrix = glm::rotate(model_matrix, rotation_radian.z, glm::vec3(0.0f, 0.0f, 1.0f));

                shader->setMat4("model", model_matrix);
                shader->setMat4("view", camera->GetViewMatrix());
                shader->setMat4("projection", camera->GetProjectionMatrix());
                shader->setVec3("eyePos", camera->Position);
                shader->setInt("shading_method", scene_config->shading_method);
                shader->setVec3("ambient_color", scene_config->ambient_color);

                shader->setBool("render_shadow", scene_config->render_shadow);
                shader->setInt("shadow_method", scene_config->shadow_method);

                int point_light_num = point_light_list.size();
                for (int i = 0; i < point_light_num; i++) {
                    auto &light = point_light_list[i];
                    std::string light_idx = "point_light[" + std::to_string(i) + "].";

                    shader->setVec3(light_idx + "position", light->position);
                    shader->setVec3(light_idx + "color", light->color);
                    shader->setFloat(light_idx + "radius", light->radius);
                    shader->setFloat(light_idx + "intensity", light->intensity);
                }

                int directional_light_num = directional_light_list.size();
                for (int i = 0; i < directional_light_num; i++) {
                    auto &light = directional_light_list[i];
                    std::string light_idx = "directional_light[" + std::to_string(i) + "].";

                    glm::vec3 direction = euler_to_direction(light->direction);

                    shader->setVec3(light_idx + "direction", direction);
                    shader->setVec3(light_idx + "color", light->color);
                    shader->setFloat(light_idx + "intensity", light->intensity);

                    if (scene_config->render_shadow) {
                        shader->setMat4("directional_light_matrix[" + std::to_string(i) + "]", directional_light_matrix_list[i]);
                        auto shadow_map_name = "directional_shadow_map[" + std::to_string(i) + "]";

                        glActiveTexture(GL_TEXTURE0 + 13 + i);
                        glUniform1i(glGetUniformLocation(shader->ID, shadow_map_name.c_str()), 13 + i);
                        glBindTexture(GL_TEXTURE_2D, directional_shadow_map_list[i]->get_id());
                    }
                }

                int spot_light_num = spot_light_list.size();
                for (int i = 0; i < spot_light_num; i++) {
                    auto &light = spot_light_list[i];

                    std::string light_idx = "spot_light[" + std::to_string(i) + "].";
                    glm::vec3 direction = euler_to_direction(light->direction);

                    shader->setVec3(light_idx + "position", light->position);
                    shader->setVec3(light_idx + "direction", direction);
                    shader->setFloat(light_idx + "cutoff", glm::cos(glm::radians(light->cutoff)));
                    shader->setFloat(light_idx + "outer_cutoff", glm::cos(glm::radians(light->outer_cutoff)));
                    shader->setVec3(light_idx + "color", light->color);
                    shader->setFloat(light_idx + "radius", light->radius);
                    shader->setFloat(light_idx + "intensity", light->intensity);

                    if (scene_config->render_shadow) {
                        shader->setMat4("spot_light_matrix[" + std::to_string(i) + "]", spot_light_matrix_list[i]);
                        auto shadow_map_name = "spot_shadow_map[" + std::to_string(i) + "]";

                        glActiveTexture(GL_TEXTURE0 + 15 + i);
                        glUniform1i(glGetUniformLocation(shader->ID, shadow_map_name.c_str()), 15 + i);
                        glBindTexture(GL_TEXTURE_2D, spot_shadow_map_list[i]->get_id());
                    }
                }

                shader->setInt("point_light_num", point_light_num);
                shader->setInt("directional_light_num", directional_light_num);
                shader->setInt("spot_light_num", spot_light_num);

                shader->setBool("use_ibl_data", scene_config->enable_ibl && ibl_generated);

                render_pass.render(*mesh, *material, *ibl_data);
            }
        }
    }

    if (pass_name == "light" && scene_config->render_light) {
        for (auto& light : point_light_list) {
            auto position = light->position;
            auto color = light->color;
            auto radius = light->radius;

            render_pass.prepare();
            render_pass.active();
            render_pass.reset();
            render_pass.setup_framebuffer_default(screen_width, screen_height);

            auto &material = model_cube->materials[0];
            auto &mesh = model_cube->meshes[0];
            auto &shader = render_pass.shader;

            glm::mat4 model_matrix = glm::identity<glm::mat4x4>();
            model_matrix = glm::translate(model_matrix, position);
            model_matrix = glm::scale(model_matrix, glm::vec3(radius * 0.5f));

            shader->setMat4("model", model_matrix);
            shader->setMat4("view", camera->GetViewMatrix());
            shader->setMat4("projection", camera->GetProjectionMatrix());

            shader->setVec3("lightColor", color);

            render_pass.render(*mesh, *material, *ibl_data);
        }

        for (auto& light : spot_light_list) {
            auto position = light->position;
            auto color = light->color;
            auto radius = light->radius;

            render_pass.prepare();
            render_pass.active();
            render_pass.reset();
            render_pass.setup_framebuffer_default(screen_width, screen_height);

            auto &material = model_cube->materials[0];
            auto &mesh = model_cube->meshes[0];
            auto &shader = render_pass.shader;

            glm::mat4 model_matrix = glm::identity<glm::mat4x4>();
            auto rotation_radian = glm::radians(light->direction);
            model_matrix = glm::translate(model_matrix, position);
            model_matrix = glm::scale(model_matrix, glm::vec3(radius * 0.5f));
            model_matrix = glm::rotate(model_matrix, rotation_radian.x, glm::vec3(-1.0f, 0.0f, 0.0f));
            model_matrix = glm::rotate(model_matrix, rotation_radian.y, glm::vec3(0.0f, 0.0f, -1.0f));

            shader->setMat4("model", model_matrix);
            shader->setMat4("view", camera->GetViewMatrix());
            shader->setMat4("projection", camera->GetProjectionMatrix());

            shader->setVec3("lightColor", color);

            render_pass.render(*mesh, *material, *ibl_data);
        }
    }
}

void Scene::save_output() {
    auto width = screen_width;
    auto height = screen_height;

    unsigned char* pixels = new unsigned char[width * height * 3];
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

    // Flip image vertically while saving.
    unsigned char* pixels_flipped = new unsigned char[width * height * 3];
    for (int y = 0; y < height; ++y) {
        memcpy(pixels_flipped + (height - 1 - y) * width * 3, pixels + y * width * 3, width * 3);
    }

    // Save as png file with stb_image_write.
    auto file_name = FileSystem::getPath("output.png");
    if (!stbi_write_png(file_name.c_str(), width, height, 3, pixels_flipped, width * 3)) {
        std::cerr << "Failed to write framebuffer to file: " << file_name << std::endl;
    } else {
        std::cout << "Framebuffer saved successfully to " << file_name << std::endl;
    }

    delete[] pixels;
    delete[] pixels_flipped;
}
