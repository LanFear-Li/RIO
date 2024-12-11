#pragma once

#include "render/pass.hpp"

class Pass_Shade final : public Pass
{
public:
    Pass_Shade(const std::string &pass_name, std::shared_ptr<Scene> scene_ptr, bool is_comp=false);

    void render_pass() override;

    void setup_framebuffer_shade(int width, int height);

private:
    std::unique_ptr<Pass> pass_skybox;
    std::unique_ptr<Pass> pass_light;
};
