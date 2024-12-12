#pragma once

#include "render/pass.hpp"

class Pass_FXAA final : public Pass
{
public:
    Pass_FXAA(const std::string &pass_name, std::shared_ptr<Scene> scene_ptr, bool is_comp=false);

    void render_pass() override;
};
