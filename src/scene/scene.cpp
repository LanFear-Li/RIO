#include "scene.hpp"

#include "io/filesystem.hpp"

#include <nlohmann/json.hpp>

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

    // Load light from scene.
    auto light_json = scene_json["lights"];
    for (auto& [name, config] : light_json.items()) {
        auto light = std::make_unique<PointLight>();

        light->position = json_to_vec3(config["position"]);
        light->color = json_to_vec3(config["color"]);
        light->radius = config["radius"].get<float>();
        light->intensity = config["intensity"].get<float>();

        point_light_list.push_back(std::move(light));
    }
}

void Scene::load_model_to_scene(std::string scene_name)
{
    std::string file_path = FileSystem::getPath("runtime/assets/scenes/" + scene_name + ".json");
    auto scene_json = nlohmann::json::parse(std::ifstream(file_path), nullptr, false);
    if (scene_json.is_discarded()) {
        std::cout << "Initialize scene failed, check your scene config please." << std::endl;
        assert(0);
    }

    // Load Model from scene.
    auto model_json = scene_json["models"];
    for (auto& [name, config] : model_json.items()) {
        std::string model_name = config["name"].get<std::string>();
        std::string model_path = "runtime/assets/models/" + model_name + "/" + model_name + ".obj";
        model_path = FileSystem::getPath(model_path);

        auto model = std::make_unique<Model>(model_path);
        model_list.push_back(std::move(model));
    }
}

void Scene::render(Pass &render_pass)
{
    for (auto &model : model_list) {
        for (auto &mesh : model->meshes) {
            render_pass.prepare();
            render_pass.active();

            auto &material = model->materials[mesh.materialIndex];
            auto &shader = render_pass.shader;

            glm::mat4 model_matrix = glm::identity<glm::mat4x4>();
            shader->setMat4("model", model_matrix);
            shader->setMat4("view", camera->GetViewMatrix());
            shader->setMat4("projection", camera->GetProjectionMatrix());

            shader->setVec3("eyePos", camera->Position);

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

            render_pass.render(mesh, material);
        }
    }
}
