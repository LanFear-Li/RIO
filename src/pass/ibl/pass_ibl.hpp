#pragma once

#include "render/pass.hpp"

class Pass_IBL final : public Pass
{
public:
    Pass_IBL(const std::string &pass_name, std::shared_ptr<Scene> scene_ptr, bool is_comp=false, bool no_shader=false);

    void render_pass() override;

private:
    // Convert Equirectangular Map to Cube Map.
    std::unique_ptr<Pass> pass_rect_to_cube;
    // Irradiance Map for diffuse IBL.
    std::unique_ptr<Pass> pass_ibl_irradiance;
    // Pre-Filtered Environment Map for specular IBL.
    std::unique_ptr<Pass> pass_ibl_prefiltered_map;
    // Precomputed Environment BRDF for specular IBL.
    std::unique_ptr<Pass> pass_ibl_precomputed_brdf;
};
