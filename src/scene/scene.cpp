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

void Scene::render(Pass &render_pass)
{
    for (auto &model : model_list) {
        render_pass.prepare();
        render_pass.active();

        auto &shader = render_pass.shader;

        glm::mat4 mat_model = glm::mat4(1.0f);
        mat_model = glm::translate(mat_model, glm::vec3(0.0f, 0.0f, 0.0f));
        mat_model = glm::scale(mat_model, glm::vec3(1.0f, 1.0f, 1.0f));
        shader->setMat4("model", mat_model);
        shader->setMat4("view", camera->GetViewMatrix());
        shader->setMat4("projection", camera->GetProjectionMatrix());
        shader->setVec3("viewPos", camera->Position);

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

        glm::vec3 lightPos(1.0f, 10.0f, 5.0f);
        shader->setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        shader->setVec3("lightPos", lightPos);

        render_pass.render(*model);
    }
}
