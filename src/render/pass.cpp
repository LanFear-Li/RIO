#include "pass.hpp"

#include "io/filesystem.hpp"
#include "platform/api-function.hpp"

#include <iostream>

Pass::Pass(const std::string &pass_name, std::shared_ptr<Scene> scene_ptr, bool is_comp, bool no_shader)
{
    if (!no_shader) {
        if (is_comp) {
            std::string comp_path = "runtime/shaders/pass/" + pass_name + ".comp";

            shader = std::make_unique<Shader>(File_System::get_path(comp_path).c_str());
        } else {
            std::string vert_path = "runtime/shaders/pass/" + pass_name + ".vert";
            std::string frag_path = "runtime/shaders/pass/" + pass_name + ".frag";

            shader = std::make_unique<Shader>(File_System::get_path(vert_path).c_str(), File_System::get_path(frag_path).c_str());
        }
    }

    name = pass_name;
    scene = scene_ptr;

    profile_info = std::make_shared<Pass_Profile_Info>();
    time_query = std::make_unique<Time_Query>();
}

void Pass::prepare() const
{
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

void Pass::render()
{
    const auto total_start{std::chrono::steady_clock::now()};
    time_query->query_start();

    prepare();
    shader_reset();

    // Render pass logic is modified by each pass.
    render_pass();

    // Total time & GPU time cost profile.
    profile_info->cost_gpu_ms = time_query->query_end();

    const auto total_end{std::chrono::steady_clock::now()};
    profile_info->cost_total_ms = std::chrono::duration_cast<std::chrono::microseconds>(total_end - total_start).count() / 1000.0f;

    scene->pass_data_map[name] = profile_info;
}

void Pass::setup_framebuffer(int width, int height, Texture_Type type, bool mipmap)
{
    frame_buffer = std::make_shared<Frame_Buffer>();
    output = create_texture(type, width, height, mipmap);
    frame_buffer->attach_texture(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, output->get_id());
    frame_buffer->unbind();

    buffer_width = width;
    buffer_height = height;
}

void Pass::setup_framebuffer_with_copy(int width, int height, std::shared_ptr<Frame_Buffer> buffer)
{
    frame_buffer = buffer;
    buffer_width = width;
    buffer_height = height;
}

void Pass::shader_reset() const
{
    if (shader == nullptr) {
        return;
    }

    shader->use();

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

void Pass::clear() const
{
    frame_buffer->bind();
    Api_Function::clear_color_and_depth();
    frame_buffer->unbind();
}

void Pass::render_color(const Mesh &mesh, const Material &material, const IBL_Data &ibl_data)
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

    auto tex_state = false;

    tex_state = material.normal_map != nullptr;
    frame_buffer->bind_texture(GL_TEXTURE_2D, shader->ID, tex_state ? material.normal_map->get_id() : 0, "_texture_normal");
    shader->setBool("use_normal_map", tex_state);

    tex_state = material.ambient_map != nullptr;
    frame_buffer->bind_texture(GL_TEXTURE_2D, shader->ID, tex_state ? material.ambient_map->get_id() : 0, "_texture_ambient");
    shader->setBool("use_ambient_map", tex_state);

    tex_state = material.diffuse_map != nullptr;
    frame_buffer->bind_texture(GL_TEXTURE_2D, shader->ID, tex_state ? material.diffuse_map->get_id() : 0, "_texture_diffuse");
    shader->setBool("use_diffuse_map", tex_state);

    tex_state = material.specular_map != nullptr;
    frame_buffer->bind_texture(GL_TEXTURE_2D, shader->ID, tex_state ? material.specular_map->get_id() : 0, "_texture_specular");
    shader->setBool("use_specular_map", tex_state);

    tex_state = material.emissive_map != nullptr;
    frame_buffer->bind_texture(GL_TEXTURE_2D, shader->ID, tex_state ? material.emissive_map->get_id() : 0, "_texture_emissive");
    shader->setBool("use_emissive_map", tex_state);

    tex_state = material.metallic_map != nullptr;
    frame_buffer->bind_texture(GL_TEXTURE_2D, shader->ID, tex_state ? material.metallic_map->get_id() : 0, "_texture_metallic");
    shader->setBool("use_metallic_map", tex_state);

    tex_state = material.roughness_map != nullptr;
    frame_buffer->bind_texture(GL_TEXTURE_2D, shader->ID, tex_state ? material.roughness_map->get_id() : 0, "_texture_roughness");
    shader->setBool("use_roughness_map", tex_state);

    tex_state = material.ao_map != nullptr;
    frame_buffer->bind_texture(GL_TEXTURE_2D, shader->ID, tex_state ? material.ao_map->get_id() : 0, "_texture_ao");
    shader->setBool("use_ao_map", tex_state);

    tex_state = material.metal_roughness_map != nullptr;
    frame_buffer->bind_texture(GL_TEXTURE_2D, shader->ID, tex_state ? material.metal_roughness_map->get_id() : 0, "_texture_metal_roughness");
    shader->setBool("use_metal_roughness_map", tex_state);

    // Bind IBL data as texture.
    tex_state = ibl_data.irrandiance_map != nullptr;
    frame_buffer->bind_texture(GL_TEXTURE_CUBE_MAP, shader->ID, tex_state ? ibl_data.irrandiance_map->get_id() : 0, "irrandiance_map");

    tex_state = ibl_data.prefiltered_map != nullptr;
    frame_buffer->bind_texture(GL_TEXTURE_CUBE_MAP, shader->ID, tex_state ? ibl_data.prefiltered_map->get_id() : 0, "prefiltered_map");

    tex_state = ibl_data.precomputed_brdf != nullptr;
    frame_buffer->bind_texture(GL_TEXTURE_2D, shader->ID, tex_state ? ibl_data.precomputed_brdf->get_id() : 0, "precomputed_brdf");

    Api_Function::set_viewport(buffer_width, buffer_height);

    frame_buffer->bind();
    mesh.vertex_array->bind();
    Api_Function::draw_elements(GL_TRIANGLES, static_cast<unsigned int>(mesh.indices.size()), GL_UNSIGNED_INT, 0);
    mesh.vertex_array->unbind();
    frame_buffer->unbind();
    frame_buffer->reset_active_id();
}

void Pass::render_cube(const Mesh &mesh, const Material &material)
{
    Api_Function::set_viewport(buffer_width, buffer_height);

    frame_buffer->bind();
    mesh.vertex_array->bind();
    Api_Function::draw_arrays(GL_TRIANGLES, 0, 36);
    mesh.vertex_array->unbind();
    frame_buffer->unbind();
    frame_buffer->reset_active_id();
}

void Pass::render_cubemap(const Mesh &mesh, const Texture &texture)
{
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
        Api_Function::clear_color_and_depth();
        Api_Function::draw_arrays(GL_TRIANGLES, 0, 36);
        mesh.vertex_array->unbind();
    }

    frame_buffer->unbind();
    frame_buffer->reset_active_id();
}

void Pass::render_quad(const Mesh &mesh)
{
    Api_Function::set_viewport(buffer_width, buffer_height);

    frame_buffer->bind();
    mesh.vertex_array->bind();
    Api_Function::clear_color_and_depth();
    Api_Function::draw_arrays(GL_TRIANGLE_STRIP, 0, 4);
    mesh.vertex_array->unbind();
    frame_buffer->unbind();
}

void Pass::render_quad_post(const Mesh &mesh, const std::shared_ptr<Texture> &texture, const std::string &name)
{
    frame_buffer->bind_texture(GL_TEXTURE_2D, shader->ID, texture->get_id(), name.c_str());

    Api_Function::set_viewport(buffer_width, buffer_height);

    frame_buffer->bind();
    mesh.vertex_array->bind();
    Api_Function::draw_arrays(GL_TRIANGLE_STRIP, 0, 4);
    mesh.vertex_array->unbind();
    frame_buffer->unbind();
    frame_buffer->reset_active_id();
}
