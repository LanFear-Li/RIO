#include "pass.hpp"

#include "filesystem.hpp"

void Pass::shaderInit()
{
    shader = Shader(FileSystem::getPath("runtime/shaders/shader.vert").c_str(), FileSystem::getPath("runtime/shaders/shader.frag").c_str());
}

void Pass::active()
{
    shader.use();
}

void Pass::render(Model &model)
{
    glm::mat4 mat_model = glm::mat4(1.0f);
    mat_model = glm::translate(mat_model, glm::vec3(0.0f, 0.0f, 0.0f));
    mat_model = glm::scale(mat_model, glm::vec3(1.0f, 1.0f, 1.0f));
    shader.setMat4("model", mat_model);
    model.Draw(shader);
}
