#pragma once

#include "render/pass.hpp"

class Pass_Shadow final : public Pass
{
public:
    Pass_Shadow(const std::string &pass_name, std::shared_ptr<Scene> scene_ptr, bool is_comp=false);

    void render_pass() override;
};
