#include "loader.hpp"

#include "model.hpp"

bool load_model_to_scene(Scene& scene, const std::string &path)
{
    auto model = new Model(path);
    scene.model_list.emplace_back(model);

    return true;
}
