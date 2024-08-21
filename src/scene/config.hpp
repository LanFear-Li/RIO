#pragma once

#include <iostream>
#include <string>

struct Panel_Config {
    std::string model_name;

    bool show_skybox = true;
    bool enable_ibl = false;
    std::string skybox_name;

    bool render_light = true;
    bool render_shadow = false;
};
