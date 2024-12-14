#pragma once

#include "render/pass.hpp"

class Pass_Neighborhood_Blending final : public Pass
{
public:
    Pass_Neighborhood_Blending(const std::string &pass_name, std::shared_ptr<Scene> scene_ptr, bool is_comp=false);

    void render_pass() override;

    void render_smaa(const Mesh &mesh);

    std::shared_ptr<Texture> tex_blend;
};
