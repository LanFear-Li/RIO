#pragma once

#include <glm/glm.hpp>
#include <nlohmann/json.hpp>

#include <iostream>
#include <string>
#include <vector>

using json = nlohmann::json;

enum Shading_Method {
    PHONG, BLINN_PHONG, BRDF
};

NLOHMANN_JSON_SERIALIZE_ENUM(Shading_Method, {
    { PHONG, "PHONG" },
    { BLINN_PHONG, "BLINN_PHONG" },
    { BRDF, "BRDF" }
})

const std::vector<const char *> shading_methods = {
    "PHONG", "BLINN_PHONG", "BRDF"
};

enum Shadow_Method {
    BASIC, PCF, PCSS
};

NLOHMANN_JSON_SERIALIZE_ENUM(Shadow_Method, {
    { BASIC, "BASIC" },
    { PCF, "PCF" },
    { PCSS, "PCSS" }
})

const std::vector<const char *> shadow_methods = {
    "BASIC", "PCF", "PCSS"
};

struct Panel_Config {
    // Render config.
    bool show_skybox = true;
    bool enable_ibl = false;

    bool render_light = true;
    bool render_shadow = true;
    bool render_gui = true;

    Shading_Method shading_method{Shading_Method::BRDF};
    Shadow_Method shadow_method{Shadow_Method::PCSS};

    // Scene config.
    std::string model_name;
    std::string skybox_name;

    glm::vec3 ambient_color{0.5f, 0.5f, 0.5f};
};
