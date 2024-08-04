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

    auto camera_json = scene_json["camera"];
    auto pos_json = camera_json["pos"];
    glm::vec3 pos(pos_json[0].get<float>(), pos_json[1].get<float>(), pos_json[2].get<float>());

    camera = std::make_unique<Camera>(pos);

    if (camera_json.contains("resolution")) {
        camera->cameraWidth = camera_json["resolution"][0].get<uint32_t>();
        camera->cameraHeight = camera_json["resolution"][1].get<uint32_t>();
    }
}

void Scene::render(Pass &render_pass)
{
    for (auto &model : model_list) {
        glm::vec3 lightPos(1.0f, 10.0f, 5.0f);

        render_pass.prepare();
        render_pass.active();

        auto &shader = render_pass.shader;
        shader.setMat4("projection", camera->GetProjectionMatrix());
        shader.setMat4("view", camera->GetViewMatrix());

        shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        shader.setVec3("lightPos", lightPos);
        shader.setVec3("viewPos", camera->Position);
        render_pass.render(*model);
    }
}
