#pragma once

#include "render/pass.hpp"

class Pass_Blend_Weight_Calculation final : public Pass
{
public:
    Pass_Blend_Weight_Calculation(const std::string &pass_name, std::shared_ptr<Scene> scene_ptr, bool is_comp=false);

    void render_pass() override;

    void render_smaa(const Mesh &mesh);

    std::shared_ptr<Texture> tex_edge;
    std::shared_ptr<Texture> tex_area;
    std::shared_ptr<Texture> tex_search;

    std::shared_ptr<Texture> tex_blend;
};
