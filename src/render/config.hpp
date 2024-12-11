#pragma once

#include <glm/glm.hpp>
#include <nlohmann/json.hpp>

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
    BASIC, PCF, PCSS, VSM, VSSM
};

NLOHMANN_JSON_SERIALIZE_ENUM(Shadow_Method, {
    { BASIC, "BASIC" },
    { PCF, "PCF" },
    { PCSS, "PCSS" },
    { VSM, "VSM" },
    { VSSM, "VSSM" }
})

const std::vector<const char *> shadow_methods = {
    "BASIC", "PCF", "PCSS", "VSM", "VSSM"
};

enum AA_Method {
    NONE, FXAA, SMAA, TAA
};

NLOHMANN_JSON_SERIALIZE_ENUM(AA_Method, {
    { NONE, "NONE" },
    { FXAA, "FXAA" },
    { SMAA, "SMAA" },
    { TAA, "TAA" }
})

const std::vector<const char *> aa_methods = {
    "NONE", "FXAA", "SMAA", "TAA"
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
    AA_Method aa_method{AA_Method::FXAA};

    // Scene config.
    std::string model_name;
    std::string skybox_name;

    glm::vec3 ambient_color{0.5f, 0.5f, 0.5f};
};
