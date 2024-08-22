#pragma once

#include <iostream>
#include <string>
#include <vector>

enum Shading_Model {
    PHONG,
    BLINN_PHONG,
    BRDF
};

const std::vector<const char *> ShadingModelNames = {
    "PHONG",
    "BLINN_PHONG",
    "BRDF"
};

struct Panel_Config {
    // Render config.
    bool show_skybox = true;
    bool enable_ibl = false;
    std::string skybox_name;

    bool render_light = true;
    bool render_shadow = false;

    Shading_Model shading_mode{Shading_Model::BRDF};

    // Scene config.
    std::string model_name;
};
