#pragma once

#include "render/pass.hpp"

class Pass_IBL_Irradiance final : public Pass
{
public:
    Pass_IBL_Irradiance(const std::string &pass_name, std::shared_ptr<Scene> scene_ptr, bool is_comp=false);

    void render_pass() override;
};
