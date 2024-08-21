#pragma once

#include "scene/config.hpp"
#include "scene/scene.hpp"

#include <iostream>
#include <string>

struct Panel {
public:
    Panel(void *window, std::shared_ptr<Scene> scene);
    void init_config(Panel_Config &config);
    void render();

private:
    Panel_Config panel_config{};
    std::shared_ptr<Scene> scene;
};
