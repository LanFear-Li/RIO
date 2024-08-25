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

void Pass::setup_framebuffer(int width, int height)
{
    glGenFramebuffers(1, &fbo);
    glGenRenderbuffers(1, &rbo);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    output = create_texture(Texture_Type::TEXTURE_CUBE_MAP, width, height);
    buffer_width = width;
    buffer_height = height;
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
    shader->setBool("use_metal_roughness_map", false);
    shader->setBool("use_ao_map", false);

    shader->setBool("use_ibl_data", false);
}

void Pass::render(Mesh &mesh, Material &material, IBL_Data &ibl_data)
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

    if (material.ao_map != nullptr) {
        glActiveTexture(GL_TEXTURE0 + texture_idx);
        glUniform1i(glGetUniformLocation(shader->ID, "_texture_ao"), texture_idx);
        glBindTexture(GL_TEXTURE_2D, material.ao_map->get_id());
        shader->setBool("use_ao_map", true);
        texture_idx++;
    }

    if (material.metal_roughness_map != nullptr) {
        glActiveTexture(GL_TEXTURE0 + texture_idx);
        glUniform1i(glGetUniformLocation(shader->ID, "_texture_metal_roughness"), texture_idx);
        glBindTexture(GL_TEXTURE_2D, material.metal_roughness_map->get_id());
        shader->setBool("use_metal_roughness_map", true);
        texture_idx++;
    }

    // Bind IBL data as texture.
    if (ibl_data.irrandiance_map != nullptr) {
        glActiveTexture(GL_TEXTURE0 + texture_idx);
        glUniform1i(glGetUniformLocation(shader->ID, "irrandiance_map"), texture_idx);
        glBindTexture(GL_TEXTURE_CUBE_MAP, ibl_data.irrandiance_map->get_id());
        texture_idx++;
    }

    // Specially for pass_skybox.
    if (name == "skybox") {
        if (material.ibl_map != nullptr) {
            glActiveTexture(GL_TEXTURE0 + texture_idx);
            glUniform1i(glGetUniformLocation(shader->ID, "_texture_ibl"), texture_idx);
            glBindTexture(GL_TEXTURE_CUBE_MAP, material.ibl_map->get_id());
            texture_idx++;
        }
    }

    // Draw mesh via VAO.
    glBindVertexArray(mesh.VAO);
    if (name == "skybox" || name == "light") {
        glDrawArrays(GL_TRIANGLES, 0, 36);
    } else {
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(mesh.indices.size()), GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);

    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
}

void Pass::render_cubemap(Mesh &mesh, Texture &texture)
{
    unsigned int texture_idx = 0;

    if (name == "rect_to_cube") {
        glActiveTexture(GL_TEXTURE0 + texture_idx);
        glUniform1i(glGetUniformLocation(shader->ID, "equirectangular_map"), texture_idx);
        glBindTexture(GL_TEXTURE_2D, texture.get_id());
    } else if (name == "ibl_irradiance") {
        glActiveTexture(GL_TEXTURE0 + texture_idx);
        glUniform1i(glGetUniformLocation(shader->ID, "environment_map"), texture_idx);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture.get_id());
    }

    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] = {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    shader->setMat4("projection", captureProjection);

    // Record current viewport for restore.
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    glViewport(0, 0, buffer_width, buffer_height);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    for (unsigned int i = 0; i < 6; ++i) {
        shader->setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, output->get_id(), 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw mesh via VAO.
        glBindVertexArray(mesh.VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glActiveTexture(GL_TEXTURE0);

    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}
