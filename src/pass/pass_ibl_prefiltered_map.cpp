#include "pass_ibl_prefiltered_map.hpp"

Pass_IBL_Prefiltered_Map::Pass_IBL_Prefiltered_Map(const std::string &pass_name, std::shared_ptr<Scene> scene_ptr, bool is_comp)
: Pass(pass_name, scene_ptr, is_comp) {}

void Pass_IBL_Prefiltered_Map::render_pass()
{
    if (scene->scene_config->enable_ibl && !scene->ibl_generated) {
        setup_framebuffer(128, 128, Texture_Type::TEXTURE_CUBE_MAP ,true);

        auto &mesh = scene->model_cube->meshes[0];
        render_cubemap_mipmap(*mesh, *scene->ibl_data->environment_map);

        scene->ibl_data->prefiltered_map = std::move(output);
    }
}
