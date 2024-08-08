#include "loader.hpp"

#include <nlohmann/json.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void load_model_to_scene(Scene& scene, const std::string &path)
{
    auto model = std::make_unique<Model>(path);
    scene.model_list.push_back(std::move(model));
}
