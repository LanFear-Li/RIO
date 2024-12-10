#pragma once

#include "render/pass.hpp"

class Pass_Compute_SAT final : public Pass
{
public:
    Pass_Compute_SAT(const std::string &pass_name, std::shared_ptr<Scene> scene_ptr, bool is_comp=false);

    void render_pass() override;

    void setup_framebuffer_comp_SAT(int width, int height);
    void render_comp_SAT(GLuint shadow_map);

private:
    std::unique_ptr<Texture> SAT_map[2];
};
