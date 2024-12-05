#pragma once

#include "render/pass.hpp"

class Pass_Rect_To_Cube final : public Pass
{
public:
    Pass_Rect_To_Cube(const std::string &pass_name, std::shared_ptr<Scene> scene_ptr, bool is_comp=false);

    void render_pass() override;

    void render_cubemap_rect(const Mesh &mesh, const Texture &texture);
};
