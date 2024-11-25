#include "pass.hpp"

#include "io/filesystem.hpp"
#include "platform/api-function.hpp"

#include <iostream>

Pass::Pass(const std::string &pass_name)
{
    std::string vert_path = "runtime/shaders/pass/" + pass_name + ".vert";
    std::string frag_path = "runtime/shaders/pass/" + pass_name + ".frag";

    shader = std::make_unique<Shader>(File_System::get_path(vert_path).c_str(), File_System::get_path(frag_path).c_str());
    name = pass_name;
}

void Pass::prepare() const
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
    frame_buffer = std::make_unique<Frame_Buffer>();
    render_buffer = std::make_unique<Render_Buffer>(width, height, GL_DEPTH_COMPONENT24);

    frame_buffer->attach_render_buffer(GL_DEPTH_ATTACHMENT, render_buffer->get_id());
    frame_buffer->unbind();
    render_buffer->unbind();

    output = create_texture(type, width, height, mipmap);
    buffer_width = width;
    buffer_height = height;
}

void Pass::setup_framebuffer_depth(int width, int height)
{
    output = create_texture(Texture_Type::TEXTURE_2D_DEPTH, width, height, false);

    frame_buffer = std::make_unique<Frame_Buffer>();
    frame_buffer->attach_texture(GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, output->get_id());
    frame_buffer->unbind();

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

void Pass::active() const
{
    shader->use();
}

void Pass::reset() const
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

void Pass::clear_depth() const
{
    frame_buffer->bind();
    Api_Function::clear_depth();
    frame_buffer->unbind();
}

void Pass::render(const Mesh &mesh, const Material &material, const IBL_Data &ibl_data)
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

        // frame_buffer->bind_texture(GL_TEXTURE_2D, shader->ID, material.normal_map->get_id(), "_texture_normal");
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

    Api_Function::set_viewport(buffer_width, buffer_height);

    mesh.vertex_array->bind();
    if (name == "skybox" || name == "light") {
        Api_Function::draw_arrays(GL_TRIANGLES, 0, 36);
    } else {
        Api_Function::draw_elements(GL_TRIANGLES, static_cast<unsigned int>(mesh.indices.size()), GL_UNSIGNED_INT, 0);
    }
    mesh.vertex_array->unbind();

    // always good practice to set everything back to defaults once configured.
    frame_buffer->reset_active_id();
}

void Pass::render_depth(const Mesh &mesh)
{
    Api_Function::set_viewport(buffer_width, buffer_height);

    frame_buffer->bind();
    mesh.vertex_array->bind();
    Api_Function::draw_elements(GL_TRIANGLES, static_cast<unsigned int>(mesh.indices.size()), GL_UNSIGNED_INT, 0);
    mesh.vertex_array->unbind();
    frame_buffer->unbind();
}

void Pass::render_cubemap(const Mesh &mesh, const Texture &texture)
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

    Api_Function::set_viewport(buffer_width, buffer_height);

    frame_buffer->bind();
    for (unsigned int i = 0; i < 6; ++i) {
        shader->setMat4("view", captureViews[i]);
        frame_buffer->attach_texture(GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, output->get_id());

        mesh.vertex_array->bind();
        Api_Function::clear();
        Api_Function::draw_arrays(GL_TRIANGLES, 0, 36);
        mesh.vertex_array->unbind();
    }
    frame_buffer->unbind();

    glActiveTexture(GL_TEXTURE0);
}

void Pass::render_quad(const Mesh &mesh)
{
    Api_Function::set_viewport(buffer_width, buffer_height);

    frame_buffer->bind();
    // glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    // glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, buffer_width, buffer_height);
    // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, output->get_id(), 0);
    frame_buffer->attach_texture(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, output->get_id());

    mesh.vertex_array->bind();
    Api_Function::clear();
    Api_Function::draw_arrays(GL_TRIANGLE_STRIP, 0, 4);
    mesh.vertex_array->unbind();
    frame_buffer->unbind();
}

void Pass::render_cubemap_mipmap(const Mesh &mesh, const Texture &texture)
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

    frame_buffer->bind();
    unsigned int max_mip_levels = 5;
    for (unsigned int mip = 0; mip < max_mip_levels; ++mip) {
        // Reisze framebuffer according to mip-level size.
        unsigned int mip_width  = static_cast<unsigned int>(128 * std::pow(0.5, mip));
        unsigned int mip_height = static_cast<unsigned int>(128 * std::pow(0.5, mip));

        // TODO: Bind RBO size here don't take effect?
        // glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        // glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mip_width, mip_height);
        Api_Function::set_viewport(mip_width, mip_height);

        float roughness = (float) mip / (float) (max_mip_levels - 1);
        shader->setFloat("_ibl_roughness", roughness);
        shader->setFloat("_ibl_resolution", mip_width);

        for (unsigned int i = 0; i < 6; ++i) {
            shader->setMat4("view", captureViews[i]);
            frame_buffer->attach_texture(GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, output->get_id(), mip);

            mesh.vertex_array->bind();
            Api_Function::clear();
            Api_Function::draw_arrays(GL_TRIANGLES, 0, 36);
            mesh.vertex_array->unbind();
        }
    }

    frame_buffer->unbind();
}
