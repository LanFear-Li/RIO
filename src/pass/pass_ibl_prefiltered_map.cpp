#include "pass_ibl_prefiltered_map.hpp"

#include "platform/api-function.hpp"

Pass_IBL_Prefiltered_Map::Pass_IBL_Prefiltered_Map(const std::string &pass_name, std::shared_ptr<Scene> scene_ptr, bool is_comp)
: Pass(pass_name, scene_ptr, is_comp) {}

void Pass_IBL_Prefiltered_Map::render_pass()
{
    if (scene->scene_config->enable_ibl && !scene->ibl_generated) {
        setup_framebuffer(128, 128, Texture_Type::TEXTURE_CUBE_MAP ,true);

        auto &mesh = scene->model_cube->meshes[0];
        render_cubemap_mipmap(*mesh, *scene->ibl_data->environment_map);

        scene->ibl_data->prefiltered_map = std::move(output);
    }
}

void Pass_IBL_Prefiltered_Map::render_cubemap_mipmap(const Mesh &mesh, const Texture &texture)
{
    unsigned int texture_idx = 0;

    if (name == "ibl_prefiltered_map") {
        frame_buffer->bind_texture(GL_TEXTURE_CUBE_MAP, shader->ID, texture.get_id(), "environment_map");
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
            Api_Function::clear_color_and_depth();
            Api_Function::draw_arrays(GL_TRIANGLES, 0, 36);
            mesh.vertex_array->unbind();
        }
    }

    frame_buffer->unbind();
    frame_buffer->reset_active_id();
}
