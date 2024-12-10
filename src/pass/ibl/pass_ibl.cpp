#include "pass_ibl.hpp"

#include "pass_rect_to_cube.hpp"
#include "pass_ibl_irradiance.hpp"
#include "pass_ibl_prefiltered_map.hpp"
#include "pass_ibl_precomputed_brdf.hpp"

Pass_IBL::Pass_IBL(const std::string &pass_name, std::shared_ptr<Scene> scene_ptr, bool is_comp, bool no_shader)
: Pass(pass_name, scene_ptr, is_comp, no_shader)
{
    pass_rect_to_cube = std::make_unique<Pass_Rect_To_Cube>("rect_to_cube", scene);
    pass_ibl_irradiance = std::make_unique<Pass_IBL_Irradiance>("ibl_irradiance", scene);
    pass_ibl_prefiltered_map = std::make_unique<Pass_IBL_Prefiltered_Map>("ibl_prefiltered_map", scene);
    pass_ibl_precomputed_brdf = std::make_unique<Pass_IBL_Precomputed_BRDF>("ibl_precomputed_brdf", scene);
}

void Pass_IBL::render_pass()
{
    pass_rect_to_cube->render();
    pass_ibl_irradiance->render();
    pass_ibl_prefiltered_map->render();
    pass_ibl_precomputed_brdf->render();
}
