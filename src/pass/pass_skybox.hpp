#pragma once

#include "render/pass.hpp"

class Pass_Skybox final : public Pass
{
public:
    Pass_Skybox(const std::string &pass_name, std::shared_ptr<Scene> scene_ptr, bool is_comp=false);

    void render_pass() override;
};
