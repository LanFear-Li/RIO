#pragma once

#include "scene/camera.hpp"
#include "scene/model.hpp"
#include "render/config.hpp"
#include "scene/light.hpp"
#include "render/pass.hpp"

#include <vector>
#include <memory>

constexpr int MAX_LIGHT_NUM = 2;

class Scene final
{
public:
    Scene() = delete;
    Scene(const std::string &scene_name);

    void prepare_scene(const std::string &scene_name);
    void prepare_present();
    void update_model(const std::string &model_name);
    void update_skybox(const std::string &skybox_name);

    void render(Pass &render_pass);
    void save_output() const;

    std::shared_ptr<uint32_t> screen_width;
    std::shared_ptr<uint32_t> screen_height;

    std::shared_ptr<Panel_Config> scene_config;

    // Camera & Lights.
    std::unique_ptr<Camera> camera;
    std::vector<std::unique_ptr<PointLight>> point_light_list;
    std::vector<std::unique_ptr<DirectionalLight>> directional_light_list;
    std::vector<std::unique_ptr<SpotLight>> spot_light_list;

    std::vector<std::unique_ptr<Texture>> point_shadow_map_list;
    std::vector<std::unique_ptr<Texture>> directional_shadow_map_list;
    std::vector<std::unique_ptr<Texture>> spot_shadow_map_list;

    std::vector<glm::mat4> directional_light_matrix_list;
    std::vector<glm::mat4> spot_light_matrix_list;

    // Model.
    std::vector<std::unique_ptr<Model>> model_list;
    std::vector<const char *> candidate_model_list;

    // Skybox.
    std::unique_ptr<Model> model_skybox;
    std::vector<const char *> candidate_skybox_list;

    // IBL.
    std::unique_ptr<IBL_Data> ibl_data;

    // Present Framebuffer & texture.
    std::shared_ptr<Frame_Buffer> present_fbo;
    std::shared_ptr<Texture> present_color;
    std::shared_ptr<Texture> present_depth;

    // Mesh model for pass.
    std::unique_ptr<Model> model_cube;
    std::unique_ptr<Model> model_quad;

private:
    std::string get_model_path(const std::string &model_name);

    bool cubemap_converted = false;
    bool ibl_generated = false;
};
