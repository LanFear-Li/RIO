#include "panel.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

Panel::Panel(void *window, std::shared_ptr<Scene> scene)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();

    ImGui::StyleColorsClassic();

    ImGui_ImplGlfw_InitForOpenGL((GLFWwindow *) window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    this->scene = scene;
    this->panel_config = scene->scene_config;
}

void Panel::render()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Starts form frame 0.
    scene->frame_count = ImGui::GetFrameCount() - 1;

    if (scene->scene_config->render_gui == false) {
        ImGui::Render();
        return;
    }

    // Settings begin.
    ImGui::Begin("Settings");

    ImGui::SetWindowFontScale(1.2);
    float windowWidth = ImGui::GetWindowWidth();

    // Render Statistics.
    float fps = ImGui::GetIO().Framerate;
    ImGui::Text("Total: %.2f fps / %.4f mspf", fps, 1000.0f / fps);
    if (ImGui::CollapsingHeader("Statistics (Total Time / GPU Time)", ImGuiTreeNodeFlags_None)) {
        ImGui::BeginTable("Statistics Table", 2, ImGuiTableFlags_SizingFixedSame);

        for (auto& pass_data : scene->pass_data_map) {
            auto& pass_name = pass_data.first;
            auto& profile_info = pass_data.second;

            ImGui::TableNextColumn();
            ImGui::Text("%s", pass_name.c_str());
            ImGui::TableNextColumn();
            ImGui::Text(": %.3f / %.3f mspf", profile_info->cost_total_ms, profile_info->cost_gpu_ms);
        }

        ImGui::EndTable();
    }

    // Render option.
    if (ImGui::CollapsingHeader("Render Option", ImGuiTreeNodeFlags_DefaultOpen)) {
        // Skybox & IBL.
        ImGui::Checkbox("Show Skybox", &panel_config->show_skybox);
        ImGui::SameLine();

        if (!panel_config->show_skybox) {
            panel_config->enable_ibl = false;
        }
        ImGui::Checkbox("Enable IBL", &panel_config->enable_ibl);

        // Render light.
        ImGui::Checkbox("Render Light", &panel_config->render_light);
        ImGui::SameLine();

        // TODO: Render shadow.
        ImGui::Checkbox("Render Shadow", &panel_config->render_shadow);

        // Render shading method.
        ImGui::PushItemWidth(windowWidth * 0.3f);
        auto &shading_list = shading_methods;
        int shading_idx = panel_config->shading_method;
        if (ImGui::Combo("Shading", &shading_idx, shading_list.data(), (int) shading_list.size())) {
            panel_config->shading_method = static_cast<Shading_Method>(shading_idx);
        }

        ImGui::SameLine();

        // Render shadow method.
        auto &shadow_list = shadow_methods;
        int shadow_idx = panel_config->shadow_method;
        if (ImGui::Combo("Shadow", &shadow_idx, shadow_list.data(), (int) shadow_list.size())) {
            panel_config->shadow_method = static_cast<Shadow_Method>(shadow_idx);
        }
        ImGui::PopItemWidth();

        // Skybox.
        ImGui::PushItemWidth(windowWidth * 0.3f);
        ImGui::PushID("Skybox");
        auto &skybox_list = scene->candidate_skybox_list;

        int skybox_idx = 0;
        for (; skybox_idx < skybox_list.size(); skybox_idx++) {
            if (panel_config->skybox_name == skybox_list[skybox_idx]) {
                break;
            }
        }

        if (ImGui::Combo("Skybox", &skybox_idx, skybox_list.data(), (int) skybox_list.size())) {
            panel_config->skybox_name = skybox_list[skybox_idx];
            scene->update_skybox(skybox_list[skybox_idx]);
        }
        ImGui::PopID();

        ImGui::SameLine();

        // Render aa method.
        auto &aa_list = aa_methods;
        int aa_idx = panel_config->aa_method;
        if (ImGui::Combo("AA", &aa_idx, aa_list.data(), (int) aa_list.size())) {
            panel_config->aa_method = static_cast<AA_Method>(aa_idx);
        }
        ImGui::PopItemWidth();
    }

    // Scene option.
    if (ImGui::CollapsingHeader("Scene Option", ImGuiTreeNodeFlags_DefaultOpen)) {
        // auto &model_list = scene->candidate_model_list;
        //
        // int model_idx = 0;
        // for (; model_idx < model_list.size(); model_idx++) {
        //     if (panel_config->model_name == model_list[model_idx]) {
        //         break;
        //     }
        // }
        //
        // if (ImGui::Combo("Type", &model_idx, model_list.data(), (int) model_list.size())) {
        //     panel_config->model_name = model_list[model_idx];
        //     scene->update_model(model_list[model_idx]);
        // }

        // Model.
        for (auto& model : scene->model_list) {
            ImGui::PushID(model->model_name.c_str());
            ImGui::Text(model->model_name.c_str());
            ImGui::DragFloat3("Position", (float *) &model->position, 0.1f, -1000.0f, 1000.f);
            ImGui::DragFloat3("Rotation", (float *) &model->rotation, 0.1f, -180.0f, 180.f);
            ImGui::DragFloat("Scaling", &model->scaling, 0.1f, 0.0f, 100.0f);
            ImGui::PopID();
        }

        // Scene ambient color.
        ImGui::ColorEdit3("Ambient", (float *) &panel_config->ambient_color);

        // Camera
        auto &camera = scene->camera;

        ImGui::Separator();
        ImGui::PushID("Camera");
        ImGui::Text("Camera");
        ImGui::SameLine();
        if (ImGui::Button("Reset Camera")) {
            camera->reset_camera();
        }

        ImGui::DragFloat("Speed", &camera->movement_speed, 0.1f, 0.0f, 1000.0f);
        ImGui::DragFloat3("Position", (float *) &camera->cam_position, 0.1f, -1000.0f, 1000.f);

        ImGui::PushItemWidth(windowWidth * 0.3f);
        if (ImGui::DragFloat("Pitch", &camera->cam_pitch, 0.1f, -90.0f, 90.0f)) {
            camera->update_camera_vectors();
        }
        ImGui::SameLine();
        if (ImGui::DragFloat("Yaw", &camera->cam_yaw, 0.1f, -180.0f, 0.0f)) {
            camera->update_camera_vectors();
        }
        ImGui::PopItemWidth();

        ImGui::PopID();

        // Point Light.
        for (auto& light : scene->point_light_list) {
            ImGui::Separator();

            ImGui::PushID(light->light_name.c_str());
            ImGui::Text(light->light_name.c_str());
            ImGui::DragFloat3("Position", (float *) &light->position, 0.1f, -1000.0f, -1000.f);
            ImGui::ColorEdit3("Color", (float *) &light->color);
            ImGui::DragFloat("Radius", &light->radius, 0.1f, 0.0f, 10.0f);
            ImGui::DragFloat("Intensity", &light->intensity, 0.1f, 0.0f, 1000.0f);
            ImGui::PopID();
        }

        // Directional Light.
        for (auto& light : scene->directional_light_list) {
            ImGui::Separator();

            ImGui::PushID(light->light_name.c_str());
            ImGui::Text(light->light_name.c_str());

            ImGui::PushItemWidth(windowWidth * 0.3f);
            ImGui::DragFloat("Pitch", &light->direction[0], 0.1f, 0.0f, 180.0f);
            ImGui::SameLine();
            ImGui::DragFloat("Yaw", &light->direction[1], 0.1f, -180.0f, 180.0f);
            ImGui::PopItemWidth();

            ImGui::ColorEdit3("Color", (float *) &light->color);
            ImGui::DragFloat("Intensity", &light->intensity, 0.1f, 0.0f, 1000.0f);
            ImGui::PopID();
        }

        // Spot Light.
        for (auto& light : scene->spot_light_list) {
            ImGui::Separator();

            ImGui::PushID(light->light_name.c_str());
            ImGui::Text(light->light_name.c_str());
            ImGui::DragFloat3("Position", (float *) &light->position, 0.1f, -1000.0f, -1000.f);

            ImGui::PushItemWidth(windowWidth * 0.3f);
            ImGui::DragFloat("Pitch", &light->direction[0], 0.1f, 0.0f, 180.0f);
            ImGui::SameLine();
            ImGui::DragFloat("Yaw", &light->direction[1], 0.1f, -180.0f, 180.0f);
            ImGui::PopItemWidth();

            ImGui::DragFloat("CutOff", &light->cutoff, 0.1f, 0.0f, light->outer_cutoff);
            ImGui::DragFloat("OuterCutOff", &light->outer_cutoff, 0.1f, light->cutoff, 180.0f);
            ImGui::ColorEdit3("Color", (float *) &light->color);
            ImGui::DragFloat("Radius", &light->radius, 0.1f, 0.0f, 10.0f);
            ImGui::DragFloat("Intensity", &light->intensity, 0.1f, 0.0f, 1000.0f);
            ImGui::PopID();
        }
    }

    // Settings end.
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
