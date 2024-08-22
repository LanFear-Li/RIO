#include "pass.hpp"

#include "io/filesystem.hpp"

#include <iostream>

Pass::Pass(std::string pass_name)
{
    std::string vert_path = "runtime/shaders/pass/" + pass_name + ".vert";
    std::string frag_path = "runtime/shaders/pass/" + pass_name + ".frag";

    shader = std::make_unique<Shader>(FileSystem::getPath(vert_path).c_str(), FileSystem::getPath(frag_path).c_str());
    name = pass_name;
}

void Pass::prepare()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (state_depth_test) {
        glEnable(GL_DEPTH_TEST);

        if (depth_func == Depth_Func::less_equal) {
            glDepthFunc(GL_LEQUAL);
        } else {
            glDepthFunc(GL_LESS);
        }
    } else {
        glDisable(GL_DEPTH_TEST);
    }
}

void Pass::active()
{
    shader->use();
}

void Pass::reset()
{
    shader->setBool("use_normal_map", false);
    shader->setBool("use_ambient_map", false);
    shader->setBool("use_diffuse_map", false);
    shader->setBool("use_specular_map", false);
    shader->setBool("use_emissive_map", false);
    shader->setBool("use_metallic_map", false);
    shader->setBool("use_roughness_map", false);
    shader->setBool("use_ibl_map", false);
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

    reset();

    unsigned int texture_idx = 0;
    if (material.normal_map != nullptr) {
        glActiveTexture(GL_TEXTURE0 + texture_idx);
        glUniform1i(glGetUniformLocation(shader->ID, "_texture_normal"), texture_idx);
        glBindTexture(GL_TEXTURE_2D, material.normal_map->get_id());
        shader->setBool("use_normal_map", true);
        texture_idx++;
    }

    if (material.ambient_map != nullptr) {
        glActiveTexture(GL_TEXTURE0 + texture_idx);
        glUniform1i(glGetUniformLocation(shader->ID, "_texture_ambient"), texture_idx);
        glBindTexture(GL_TEXTURE_2D, material.ambient_map->get_id());
        shader->setBool("use_ambient_map", true);
        texture_idx++;
    }

    if (material.diffuse_map != nullptr) {
        glActiveTexture(GL_TEXTURE0 + texture_idx);
        glUniform1i(glGetUniformLocation(shader->ID, "_texture_diffuse"), texture_idx);
        glBindTexture(GL_TEXTURE_2D, material.diffuse_map->get_id());
        shader->setBool("use_diffuse_map", true);
        texture_idx++;
    }

    if (material.specular_map != nullptr) {
        glActiveTexture(GL_TEXTURE0 + texture_idx);
        glUniform1i(glGetUniformLocation(shader->ID, "_texture_specular"), texture_idx);
        glBindTexture(GL_TEXTURE_2D, material.specular_map->get_id());
        shader->setBool("use_specular_map", true);
        texture_idx++;
    }

    if (material.emissive_map != nullptr) {
        glActiveTexture(GL_TEXTURE0 + texture_idx);
        glUniform1i(glGetUniformLocation(shader->ID, "_texture_emissive"), texture_idx);
        glBindTexture(GL_TEXTURE_2D, material.emissive_map->get_id());
        shader->setBool("use_emissive_map", true);
        texture_idx++;
    }

    if (material.metallic_map != nullptr) {
        glActiveTexture(GL_TEXTURE0 + texture_idx);
        glUniform1i(glGetUniformLocation(shader->ID, "_texture_metallic"), texture_idx);
        glBindTexture(GL_TEXTURE_2D, material.metallic_map->get_id());
        shader->setBool("use_metallic_map", true);
        texture_idx++;
    }

    if (material.roughness_map != nullptr) {
        glActiveTexture(GL_TEXTURE0 + texture_idx);
        glUniform1i(glGetUniformLocation(shader->ID, "_texture_roughness"), texture_idx);
        glBindTexture(GL_TEXTURE_2D, material.roughness_map->get_id());
        shader->setBool("use_roughness_map", true);
        texture_idx++;
    }

    // Specially for pass_ibl.
    if (material.ibl_map != nullptr) {
        glActiveTexture(GL_TEXTURE0 + texture_idx);
        glUniform1i(glGetUniformLocation(shader->ID, "_texture_ibl"), texture_idx);
        glBindTexture(GL_TEXTURE_2D, material.ibl_map->get_id());
        shader->setBool("use_ibl_map", true);
        texture_idx++;
    }

    // Draw mesh via VAO.
    glBindVertexArray(mesh.VAO);
    if (name == "ibl" || name == "light") {
        glDrawArrays(GL_TRIANGLES, 0, 36);
    } else {
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(mesh.indices.size()), GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);

    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
}
