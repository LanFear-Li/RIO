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

void Pass::setup_framebuffer(int width, int height, Texture_Type type, bool mipmap)
{
    glGenFramebuffers(1, &fbo);
    glGenRenderbuffers(1, &rbo);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    output = create_texture(type, width, height, mipmap);
    buffer_width = width;
    buffer_height = height;
}

void Pass::setup_framebuffer_depth(int width, int height)
{
    output = create_texture(Texture_Type::TEXTURE_2D_DEPTH, width, height, false);

    // Attach depth texture as fbo's depth buffer.
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, output->get_id(), 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    buffer_width = width;
    buffer_height = height;
}

void Pass::setup_framebuffer_default(int width, int height)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);

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

    if (material.normal_map != nullptr) {
        glActiveTexture(GL_TEXTURE0 + 0);
        glUniform1i(glGetUniformLocation(shader->ID, "_texture_normal"), 0);
        glBindTexture(GL_TEXTURE_2D, material.normal_map->get_id());
        shader->setBool("use_normal_map", true);
    }

    if (material.ambient_map != nullptr) {
        glActiveTexture(GL_TEXTURE0 + 1);
        glUniform1i(glGetUniformLocation(shader->ID, "_texture_ambient"), 1);
        glBindTexture(GL_TEXTURE_2D, material.ambient_map->get_id());
        shader->setBool("use_ambient_map", true);
    }

    if (material.diffuse_map != nullptr) {
        glActiveTexture(GL_TEXTURE0 + 2);
        glUniform1i(glGetUniformLocation(shader->ID, "_texture_diffuse"), 2);
        glBindTexture(GL_TEXTURE_2D, material.diffuse_map->get_id());
        shader->setBool("use_diffuse_map", true);
    }

    if (material.specular_map != nullptr) {
        glActiveTexture(GL_TEXTURE0 + 3);
        glUniform1i(glGetUniformLocation(shader->ID, "_texture_specular"), 3);
        glBindTexture(GL_TEXTURE_2D, material.specular_map->get_id());
        shader->setBool("use_specular_map", true);
    }

    if (material.emissive_map != nullptr) {
        glActiveTexture(GL_TEXTURE0 + 4);
        glUniform1i(glGetUniformLocation(shader->ID, "_texture_emissive"), 4);
        glBindTexture(GL_TEXTURE_2D, material.emissive_map->get_id());
        shader->setBool("use_emissive_map", true);
    }

    if (material.metallic_map != nullptr) {
        glActiveTexture(GL_TEXTURE0 + 5);
        glUniform1i(glGetUniformLocation(shader->ID, "_texture_metallic"), 5);
        glBindTexture(GL_TEXTURE_2D, material.metallic_map->get_id());
        shader->setBool("use_metallic_map", true);
    }

    if (material.roughness_map != nullptr) {
        glActiveTexture(GL_TEXTURE0 + 6);
        glUniform1i(glGetUniformLocation(shader->ID, "_texture_roughness"), 6);
        glBindTexture(GL_TEXTURE_2D, material.roughness_map->get_id());
        shader->setBool("use_roughness_map", true);
    }

    if (material.ao_map != nullptr) {
        glActiveTexture(GL_TEXTURE0 + 7);
        glUniform1i(glGetUniformLocation(shader->ID, "_texture_ao"), 7);
        glBindTexture(GL_TEXTURE_2D, material.ao_map->get_id());
        shader->setBool("use_ao_map", true);
    }

    if (material.metal_roughness_map != nullptr) {
        glActiveTexture(GL_TEXTURE0 + 8);
        glUniform1i(glGetUniformLocation(shader->ID, "_texture_metal_roughness"), 8);
        glBindTexture(GL_TEXTURE_2D, material.metal_roughness_map->get_id());
        shader->setBool("use_metal_roughness_map", true);
    }

    // Bind IBL data as texture.
    if (ibl_data.irrandiance_map != nullptr) {
        glActiveTexture(GL_TEXTURE0 + 9);
        glUniform1i(glGetUniformLocation(shader->ID, "irrandiance_map"), 9);
        glBindTexture(GL_TEXTURE_CUBE_MAP, ibl_data.irrandiance_map->get_id());
    }

    if (ibl_data.prefiltered_map != nullptr) {
        glActiveTexture(GL_TEXTURE0 + 10);
        glUniform1i(glGetUniformLocation(shader->ID, "prefiltered_map"), 10);
        glBindTexture(GL_TEXTURE_CUBE_MAP, ibl_data.prefiltered_map->get_id());
    }

    if (ibl_data.precomputed_brdf != nullptr) {
        glActiveTexture(GL_TEXTURE0 + 11);
        glUniform1i(glGetUniformLocation(shader->ID, "precomputed_brdf"), 11);
        glBindTexture(GL_TEXTURE_2D, ibl_data.precomputed_brdf->get_id());
    }

    // Specially for pass_skybox.
    if (name == "skybox" && material.skybox_map != nullptr) {
        glActiveTexture(GL_TEXTURE0 + 12);
        glUniform1i(glGetUniformLocation(shader->ID, "environment_map"), 12);
        glBindTexture(GL_TEXTURE_CUBE_MAP, material.skybox_map->get_id());
    }

    glViewport(0, 0, buffer_width, buffer_height);

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

void Pass::render_depth(Mesh &mesh)
{
    glViewport(0, 0, buffer_width, buffer_height);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindVertexArray(mesh.VAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(mesh.indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

    if (name == "rect_to_cube") {
        glBindTexture(GL_TEXTURE_CUBE_MAP, output->get_id());
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    }

    glActiveTexture(GL_TEXTURE0);
}

void Pass::render_quad(Mesh &mesh)
{
    glViewport(0, 0, buffer_width, buffer_height);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, buffer_width, buffer_height);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, output->get_id(), 0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Draw mesh via VAO.
    glBindVertexArray(mesh.VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Pass::render_cubemap_mipmap(Mesh &mesh, Texture &texture)
{
    unsigned int texture_idx = 0;

    if (name == "ibl_prefiltered_map") {
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

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    unsigned int maxMipLevels = 5;
    for (unsigned int mip = 0; mip < maxMipLevels; ++mip) {
        // Reisze framebuffer according to mip-level size.
        unsigned int mipWidth  = static_cast<unsigned int>(128 * std::pow(0.5, mip));
        unsigned int mipHeight = static_cast<unsigned int>(128 * std::pow(0.5, mip));

        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        // TODO: Bind RBO size here don't take effect?
        // glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float) mip / (float) (maxMipLevels - 1);
        shader->setFloat("_ibl_roughness", roughness);
        shader->setFloat("_ibl_resolution", mipWidth);

        for (unsigned int i = 0; i < 6; ++i) {
            shader->setMat4("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, output->get_id(), mip);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Draw mesh via VAO.
            glBindVertexArray(mesh.VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
