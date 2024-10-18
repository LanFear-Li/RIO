#pragma once

#include <glm/glm.hpp>

#include <iostream>
#include <string>
#include <vector>

enum Shading_Model {
    PHONG,
    BLINN_PHONG,
    BRDF
};

const std::vector<const char *> shading_methods = {
    "PHONG",
    "BLINN_PHONG",
    "BRDF"
};

struct Panel_Config {
    // Render config.
    bool show_skybox = true;
    bool enable_ibl = false;

    bool render_light = true;
    bool render_shadow = false;

    bool render_gui = true;

    Shading_Model shading_mode{Shading_Model::PHONG};

    // Scene config.
    std::string model_name;
    std::string skybox_name;

    glm::vec3 ambient_color{0.5f, 0.5f, 0.5f};
};
