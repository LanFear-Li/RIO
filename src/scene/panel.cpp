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
}

void Panel::init_config(Panel_Config &config) {
    panel_config = config;
}

void Panel::render() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Settings begin.
    ImGui::Begin("Settings");

    // Model Loading.
    ImGui::Text("Model Loading");
    auto &model_list = scene->candidate_model_list;

    int index = 0;
    for (; index < model_list.size(); index++) {
        if (panel_config.model_name == model_list[index]) {
            break;
        }
    }

    if (ImGui::Combo("##Model Loading", &index, model_list.data(), (int) model_list.size())) {
        panel_config.model_name = model_list[index];
        scene->update_model(model_list[index]);
    }

    // Settings end.
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
