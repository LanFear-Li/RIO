#pragma once

#include "scene/config.hpp"
#include "scene/scene.hpp"

#include <iostream>
#include <string>

class Panel final
{
public:
    Panel(void *window, std::shared_ptr<Scene> scene);
    void render();

private:
    std::shared_ptr<Scene> scene;
    std::shared_ptr<Panel_Config> panel_config;
};
