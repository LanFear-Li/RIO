#pragma once

#include "glm/glm.hpp"

#include <string>

struct PointLight
{
    std::string light_name{};
    glm::vec3 position{};
    glm::vec3 color{1.0f};

    float radius{1.0f};
    float intensity{10.0f};
};

struct DirectionalLight
{
    std::string light_name{};
    glm::vec3 direction{};
    glm::vec3 color{1.0f};

    float intensity{10.0f};
};

struct SpotLight
{
    std::string light_name{};
    glm::vec3 position{};
    glm::vec3 color{1.0f};

    float radius{1.0f};
    float intensity{10.0f};
};
