#pragma once

#include "render/pass.hpp"

class Pass_Compute_SAT final : public Pass
{
public:
    Pass_Compute_SAT(const std::string &pass_name, std::shared_ptr<Scene> scene_ptr, bool is_comp=false);

    void render_pass() override;
};
