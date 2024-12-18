#pragma once

#include "render/pass.hpp"

class Pass_TAA final : public Pass
{
public:
    Pass_TAA(const std::string &pass_name, std::shared_ptr<Scene> scene_ptr, bool is_comp=false);

    void render_pass() override;

    void render_taa(const Mesh &mesh);

private:
    std::unique_ptr<Texture> color_previous;
};
