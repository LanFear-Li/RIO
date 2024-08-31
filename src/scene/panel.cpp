#include "panel.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

Panel::Panel(void *window, std::shared_ptr<Scene> scene) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();

    ImGui::StyleColorsClassic();

    ImGui_ImplGlfw_InitForOpenGL((GLFWwindow *) window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    this->scene = scene;
    this->panel_config = scene->scene_config;
}

void Panel::render() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (scene->scene_config->render_gui == false) {
        return;
    }

    // Settings begin.
    ImGui::Begin("Settings");

    ImGui::SetWindowFontScale(1.2);

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

        // Render shading model.
        auto &shading_list = ShadingModelNames;
        int shading_idx = panel_config->shading_mode;
        if (ImGui::Combo("Shading Model", &shading_idx, shading_list.data(), (int) shading_list.size())) {
            panel_config->shading_mode = static_cast<Shading_Model>(shading_idx);
        }

        // Skybox.
        ImGui::Text("Skybox");
        ImGui::PushID("Skybox");
        auto &skybox_list = scene->candidate_skybox_list;

        int skybox_idx = 0;
        for (; skybox_idx < skybox_list.size(); skybox_idx++) {
            if (panel_config->skybox_name == skybox_list[skybox_idx]) {
                break;
            }
        }

        if (ImGui::Combo("Type", &skybox_idx, skybox_list.data(), (int) skybox_list.size())) {
            panel_config->skybox_name = skybox_list[skybox_idx];
            scene->update_skybox(skybox_list[skybox_idx]);
        }
        ImGui::PopID();
    }

    // Scene option.
    if (ImGui::CollapsingHeader("Scene Option", ImGuiTreeNodeFlags_DefaultOpen)) {
        // Model.
        ImGui::Text("Model");
        ImGui::PushID("Model");
        auto &model_list = scene->candidate_model_list;

        int model_idx = 0;
        for (; model_idx < model_list.size(); model_idx++) {
            if (panel_config->model_name == model_list[model_idx]) {
                break;
            }
        }

        if (ImGui::Combo("Type", &model_idx, model_list.data(), (int) model_list.size())) {
            panel_config->model_name = model_list[model_idx];
            scene->update_model(model_list[model_idx]);
        }

        auto &model = scene->model_list[0];
        ImGui::DragFloat3("Position", (float *) &model->position, 0.1f, -1000.0f, 1000.f);
        ImGui::DragFloat3("Rosition", (float *) &model->rotation, 0.1f, -180.0f, 180.f);
        ImGui::DragFloat("Scaling", &model->scaling, 0.1f, 0.0f, 100.0f);
        ImGui::PopID();

        // Scene ambient color.
        ImGui::ColorEdit3("Ambient", (float *) &panel_config->ambient_color);

        // Camera
        auto &camera = scene->camera;
        float windowWidth = ImGui::GetWindowWidth();

        ImGui::Separator();
        ImGui::PushID("Camera");
        ImGui::Text("Camera");
        ImGui::SameLine();
        if (ImGui::Button("Reset Camera")) {
            camera->ResetCamera();
        }

        ImGui::DragFloat("Speed", &camera->MovementSpeed, 0.1f, 0.0f, 1000.0f);
        ImGui::DragFloat3("Position", (float *) &camera->Position, 0.1f, -1000.0f, 1000.f);

        ImGui::PushItemWidth(windowWidth * 0.3f);
        if (ImGui::DragFloat("Yaw", &camera->Yaw, 0.1f, -180.0f, 0.0f)) {
            camera->updateCameraVectors();
        }
        ImGui::SameLine();
        if (ImGui::DragFloat("Pitch", &camera->Pitch, 0.1f, -90.0f, 90.0f)) {
            camera->updateCameraVectors();
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
    }

    // Settings end.
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
