#pragma once

#include "render/pass.hpp"

class Pass_Post_Process final : public Pass
{
public:
    Pass_Post_Process(const std::string &pass_name, std::shared_ptr<Scene> scene_ptr, bool is_comp=false, bool no_shader=false);

    void render_pass() override;

private:
    std::unique_ptr<Pass> pass_grading;

    std::unique_ptr<Pass> pass_fxaa;
    std::unique_ptr<Pass> pass_smaa;
};
