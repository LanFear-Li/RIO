#pragma once

#include "render/pass.hpp"

#include "pass_edge_detection.hpp"
#include "pass_blend_weight_calculation.hpp"
#include "pass_neighborhood_blending.hpp"

class Pass_SMAA final : public Pass
{
public:
    Pass_SMAA(const std::string &pass_name, std::shared_ptr<Scene> scene_ptr, bool is_comp=false, bool no_shader=false);

    void render_pass() override;

private:
    std::unique_ptr<Pass_Edge_Detection> pass_edge_detection;
    std::unique_ptr<Pass_Blend_Weight_Calculation> pass_blend_weight_calculation;
    std::unique_ptr<Pass_Neighborhood_Blending> pass_neighborhood_blending;
};
