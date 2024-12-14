#include "pass_smaa.hpp"

Pass_SMAA::Pass_SMAA(const std::string &pass_name, std::shared_ptr<Scene> scene_ptr, bool is_comp, bool no_shader)
: Pass(pass_name, scene_ptr, is_comp, no_shader)
{
    pass_edge_detection = std::make_unique<Pass_Edge_Detection>("edge_detection", scene);
    pass_blend_weight_calculation = std::make_unique<Pass_Blend_Weight_Calculation>("blend_weight_calculation", scene);
    pass_neighborhood_blending = std::make_unique<Pass_Neighborhood_Blending>("neighborhood_blending", scene);
}

void Pass_SMAA::render_pass()
{
    pass_edge_detection->render();

    pass_blend_weight_calculation->tex_edge = pass_edge_detection->tex_edge;
    pass_blend_weight_calculation->render();

    pass_neighborhood_blending->tex_blend = pass_blend_weight_calculation->tex_blend;
    pass_neighborhood_blending->render();
}
