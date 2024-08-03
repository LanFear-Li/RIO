#include "scene.hpp"

Scene::Scene() : camera(glm::vec3(0.0f, 10.0f, 15.0f)) {}

void Scene::render(Pass &render_pass)
{
    for (auto &model : model_list) {
        glm::vec3 lightPos(1.0f, 10.0f, 5.0f);

        render_pass.prepare();
        render_pass.active();

        auto &shader = render_pass.shader;
        shader.setMat4("projection", camera.GetProjectionMatrix());
        shader.setMat4("view", camera.GetViewMatrix());

        shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        shader.setVec3("lightPos", lightPos);
        shader.setVec3("viewPos", camera.Position);
        render_pass.render(*model);
    }
}
