#include "pass_ibl_precomputed_brdf.hpp"

Pass_IBL_Precomputed_BRDF::Pass_IBL_Precomputed_BRDF(const std::string &pass_name, std::shared_ptr<Scene> scene_ptr, bool is_comp)
: Pass(pass_name, scene_ptr, is_comp) {}

void Pass_IBL_Precomputed_BRDF::render_pass()
{
    if (scene->scene_config->enable_ibl && !scene->ibl_generated) {
        setup_framebuffer(512, 512, Texture_Type::TEXTURE_2D);

        auto &mesh = scene->model_quad->meshes[0];
        render_quad(*mesh);

        scene->ibl_data->precomputed_brdf = std::move(output);
        scene->ibl_generated = true;
    }
}
