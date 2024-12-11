#pragma once

#include "render/pass.hpp"

class Pass_Present final : public Pass
{
public:
    Pass_Present(const std::string &pass_name, std::shared_ptr<Scene> scene_ptr, bool is_comp=false, bool no_shader=false);

    void render_pass() override;

    void setup_framebuffer_present(int width, int height);
};
