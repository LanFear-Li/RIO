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
        int index = panel_config->shading_mode;
        if (ImGui::Combo("Shading Model", &index, shading_list.data(), (int) shading_list.size())) {
            panel_config->shading_mode = static_cast<Shading_Model>(index);
        }
    }

    // Scene option.
    if (ImGui::CollapsingHeader("Scene Option", ImGuiTreeNodeFlags_DefaultOpen)) {
        // Model.
        auto &model_list = scene->candidate_model_list;

        int index = 0;
        for (; index < model_list.size(); index++) {
            if (panel_config->model_name == model_list[index]) {
                break;
            }
        }

        if (ImGui::Combo("Model", &index, model_list.data(), (int) model_list.size())) {
            panel_config->model_name = model_list[index];
            scene->update_model(model_list[index]);
        }

        // Point Light.
        for (auto& light : scene->point_light_list) {
            ImGui::Separator();

            ImGui::PushID(light->light_name.c_str());
            ImGui::Text(light->light_name.c_str());
            ImGui::DragFloat3("Position", (float *) &light->position, 0.1f, -10.0f, 10.f);
            ImGui::ColorEdit3("Color", (float *) &light->color);
            ImGui::DragFloat("Radius", &light->radius, 0.1f, 0.0f, 10.0f);
            ImGui::DragFloat("Intensity", &light->intensity, 0.1f, 0.0f, 10.0f);
            ImGui::PopID();
        }
    }

    // Settings end.
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
