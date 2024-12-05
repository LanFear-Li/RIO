#include "pass_compute_SAT.hpp"

Pass_Compute_SAT::Pass_Compute_SAT(const std::string &pass_name, std::shared_ptr<Scene> scene_ptr, bool is_comp)
: Pass(pass_name, scene_ptr, is_comp) {}

void Pass_Compute_SAT::render_pass()
{
    if (scene->scene_config->render_shadow &&
    (scene->scene_config->shadow_method == Shadow_Method::VSM || scene->scene_config->shadow_method == Shadow_Method::VSSM)) {
        shader->setInt("input_image", 0);
        shader->setInt("output_image", 1);

        for (int i = 0; i < scene->directional_light_list.size(); i++) {
            setup_framebuffer_comp_SAT(1024, 1024);
            render_comp_SAT(scene->directional_shadow_map_list[i]->get_id());

            scene->directional_SAT_map_list[i] = std::move(SAT_map[1]);
        }

        for (int i = 0; i < scene->spot_light_list.size(); i++) {
            setup_framebuffer_comp_SAT(1024, 1024);
            render_comp_SAT(scene->spot_shadow_map_list[i]->get_id());

            scene->spot_SAT_map_list[i] = std::move(SAT_map[1]);
        }
    }
}
