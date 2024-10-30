#pragma once

#include "scene/camera.hpp"
#include "scene/model.hpp"
#include "scene/config.hpp"
#include "scene/light.hpp"
#include "render/pass.hpp"

#include <vector>
#include <memory>

struct Scene
{
public:
    Scene() = delete;
    Scene(std::string scene_name);
    void prepare_scene(std::string scene_name);

    std::string get_model_path(std::string model_name);
    void update_model(std::string model_name);
    void update_skybox(std::string skybox_name);

    void update();
    void render(Pass &render_pass);
    void save_output();

    unsigned int screen_width{};
    unsigned int screen_height{};

    bool cubemap_converted = false;
    bool ibl_generated = false;

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

    // Mesh model for pass.
    std::unique_ptr<Model> model_cube;
    std::unique_ptr<Model> model_quad;
};
