#include "scene.hpp"

#include "io/filesystem.hpp"

#include <nlohmann/json.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <filesystem>

namespace fs = std::filesystem;

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
        auto light = std::make_unique<PointLight>();

        light->light_name = name;
        light->position = json_to_vec3(config["position"]);
        light->color = json_to_vec3(config["color"]);
        light->radius = config["radius"].get<float>();
        light->intensity = config["intensity"].get<float>();

        point_light_list.push_back(std::move(light));
    }

    model_cube = Model::constructCube();
    model_quad = Model::constructQuad();

    // Load config from scene.
    scene_config->model_name = model_list.front()->model_name;

    if (model_skybox) {
        scene_config->skybox_name = model_skybox->model_name;
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

    if (pass_name == "shade") {
        for (auto &model : model_list) {
            for (auto &mesh : model->meshes) {
                render_pass.prepare();
                render_pass.active();
                render_pass.reset();
                render_pass.setup_framebuffer_default(screen_width, screen_height);

                auto &material = model->materials[mesh->materialIndex];
                auto &shader = render_pass.shader;

                auto rotation_radian = glm::radians(model->rotation);
                auto model_matrix = glm::eulerAngleXYZ(rotation_radian.x, rotation_radian.y, rotation_radian.z);
                model_matrix = glm::translate(model_matrix, model->position);
                model_matrix = glm::scale(model_matrix, glm::vec3(model->scaling));

                shader->setMat4("model", model_matrix);
                shader->setMat4("view", camera->GetViewMatrix());
                shader->setMat4("projection", camera->GetProjectionMatrix());
                shader->setVec3("eyePos", camera->Position);
                shader->setInt("shading_model", scene_config->shading_mode);
                shader->setVec3("ambient_color", scene_config->ambient_color);

                int point_light_num = point_light_list.size();
                for (int i = 0; i < point_light_num; i++) {
                    auto &light = point_light_list[i];
                    std::string light_idx = "point_light[" + std::to_string(i) + "].";

                    shader->setVec3(light_idx + "position", light->position);
                    shader->setVec3(light_idx + "color", light->color);
                    shader->setFloat(light_idx + "radius", light->radius);
                    shader->setFloat(light_idx + "intensity", light->intensity);
                }
                shader->setInt("point_light_num", point_light_num);

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
    }
}
