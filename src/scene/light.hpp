#pragma once

#include "glm/glm.hpp"

struct PointLight
{
    glm::vec3 position{};
    glm::vec3 color{1.0f};

    float radius{1.0f};
    float intensity{10.0f};
};
