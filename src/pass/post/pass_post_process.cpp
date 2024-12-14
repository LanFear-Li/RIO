#include "pass_post_process.hpp"

#include "pass_grading.hpp"
#include "pass_fxaa.hpp"
#include "pass_smaa.hpp"

Pass_Post_Process::Pass_Post_Process(const std::string &pass_name, std::shared_ptr<Scene> scene_ptr, bool is_comp, bool no_shader)
: Pass(pass_name, scene_ptr, is_comp, no_shader)
{
    pass_grading = std::make_unique<Pass_Grading>("grading", scene);
    pass_fxaa = std::make_unique<Pass_FXAA>("fxaa", scene);
    pass_smaa = std::make_unique<Pass_SMAA>("fxaa", scene, false, true);
}

void Pass_Post_Process::render_pass()
{
    pass_grading->render();

    if (scene->scene_config->aa_method == AA_Method::FXAA) {
        pass_fxaa->render();
    } else if (scene->scene_config->aa_method == AA_Method::SMAA) {
        pass_smaa->render();
    }
}
