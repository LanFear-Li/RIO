#include "loader.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

bool load_model_to_scene(Scene& scene, const std::string &path)
{
    stbi_set_flip_vertically_on_load(true);

    auto model = new Model(path);
    scene.model_list.emplace_back(model);

    return true;
}
