#pragma once

#include "scene/config.hpp"
#include "scene/scene.hpp"

#include <iostream>
#include <string>

struct Panel {
public:
    Panel(void *window, std::shared_ptr<Scene> scene);
    void render();

private:
    std::shared_ptr<Scene> scene;
    std::shared_ptr<Panel_Config> panel_config;
};
