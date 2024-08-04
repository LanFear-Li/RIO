#include "pass.hpp"

#include "io/filesystem.hpp"

Pass::Pass(std::string shader_name)
{
    std::string vert_path = "runtime/shaders/" + shader_name + ".vert";
    std::string frag_path = "runtime/shaders/" + shader_name + ".frag";
    shader = std::make_unique<Shader>(FileSystem::getPath(vert_path).c_str(), FileSystem::getPath(frag_path).c_str());
}

void Pass::prepare()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glEnable(GL_DEPTH_TEST);
}

void Pass::active()
{
    shader->use();
}

void Pass::render(Model &model)
{
    model.Draw(*shader);
}
