#include "pass_ibl_irradiance.hpp"

Pass_IBL_Irradiance::Pass_IBL_Irradiance(const std::string &pass_name, std::shared_ptr<Scene> scene_ptr, bool is_comp)
: Pass(pass_name, scene_ptr, is_comp) {}

void Pass_IBL_Irradiance::render_pass()
{
    if (scene->scene_config->enable_ibl && !scene->ibl_generated) {
        setup_framebuffer(32, 32, Texture_Type::TEXTURE_CUBE_MAP);
        auto &mesh = scene->model_cube->meshes[0];
        render_cubemap(*mesh, *scene->ibl_data->environment_map);

        scene->ibl_data->irrandiance_map = std::move(output);
    }

}
