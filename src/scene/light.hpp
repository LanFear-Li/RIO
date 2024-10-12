#pragma once

#include "glm/glm.hpp"

#include <string>

struct PointLight
{
    std::string light_name{};
    glm::vec3 color{1.0f};
    float intensity{10.0f};

    glm::vec3 position{};
    float radius{1.0f};
};

struct DirectionalLight
{
    std::string light_name{};
    glm::vec3 color{1.0f};
    float intensity{10.0f};

    glm::vec3 direction{};
};

struct SpotLight
{
    std::string light_name{};
    glm::vec3 color{1.0f};
    float intensity{10.0f};

    glm::vec3 position{};
    glm::vec3 direction{};
    float cutoff{12.5f};
    float outer_cutoff{20.0f};
    float radius{1.0f};
};
