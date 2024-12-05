#pragma once

#include "render/pass.hpp"

class Pass_IBL_Prefiltered_Map final : public Pass
{
public:
    Pass_IBL_Prefiltered_Map(const std::string &pass_name, std::shared_ptr<Scene> scene_ptr, bool is_comp=false);

    void render_pass() override;
};
