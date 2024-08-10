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

void Pass::render(Mesh &mesh, Material &material)
{
    shader->setVec3("_mat_ambient", material.ambient);
    shader->setVec3("_mat_diffuse", material.diffuse);
    shader->setVec3("_mat_specular", material.specular);
    shader->setVec3("_mat_transparency", material.transparency);
    shader->setFloat("_mat_shiness", material.shiniess);
    shader->setFloat("_mat_ior", material.ior);

    shader->setVec3("_mat_emissive", material.emissive);
    shader->setFloat("_mat_roughness", material.roughness);
    shader->setFloat("_mat_metallic", material.metallic);

    if (material.normal_map != nullptr) {
        glActiveTexture(GL_TEXTURE0 + 0);
        glUniform1i(glGetUniformLocation(shader->ID, "_texture_normal"), 1);
        glBindTexture(GL_TEXTURE_2D, material.normal_map->get_id());
        shader->setBool("use_normal_map", true);
    }

    if (material.diffuse_map != nullptr) {
        glActiveTexture(GL_TEXTURE0 + 1);
        glUniform1i(glGetUniformLocation(shader->ID, "_texture_diffuse"), 1);
        glBindTexture(GL_TEXTURE_2D, material.diffuse_map->get_id());
        shader->setBool("use_diffuse_map", true);
    }

    if (material.specular_map != nullptr) {
        glActiveTexture(GL_TEXTURE0 + 2);
        glUniform1i(glGetUniformLocation(shader->ID, "_texture_specular"), 1);
        glBindTexture(GL_TEXTURE_2D, material.specular_map->get_id());
        shader->setBool("use_specular_map", true);
    }

    if (material.emissive_map != nullptr) {
        glActiveTexture(GL_TEXTURE0 + 3);
        glUniform1i(glGetUniformLocation(shader->ID, "_texture_emissive"), 1);
        glBindTexture(GL_TEXTURE_2D, material.emissive_map->get_id());
        shader->setBool("use_emissive_map", true);
    }

    if (material.metallic_map != nullptr) {
        glActiveTexture(GL_TEXTURE0 + 4);
        glUniform1i(glGetUniformLocation(shader->ID, "_texture_metallic"), 1);
        glBindTexture(GL_TEXTURE_2D, material.metallic_map->get_id());
        shader->setBool("use_metallic_map", true);
    }

    if (material.roughness_map != nullptr) {
        glActiveTexture(GL_TEXTURE0 + 5);
        glUniform1i(glGetUniformLocation(shader->ID, "_texture_roughness"), 1);
        glBindTexture(GL_TEXTURE_2D, material.roughness_map->get_id());
        shader->setBool("use_roughness_map", true);
    }

    // Draw mesh via VAO.
    glBindVertexArray(mesh.VAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(mesh.indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
}
