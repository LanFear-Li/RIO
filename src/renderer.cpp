#include "renderer.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <utility>

Renderer::Renderer(std::string scene_name)
{
    scene = std::make_shared<Scene>(scene_name);

    auto &camera = scene->camera;
    window = std::make_unique<Window>(camera->cameraWidth, camera->cameraHeight, "RIO: Render In OpenGL");
    panel = std::make_unique<Panel>(window->glWindow, scene);

    // Initialize all render pass.
    pass_shade = std::make_unique<Pass>("shade");

    pass_ibl = std::make_unique<Pass>("ibl");
    pass_ibl->depth_func = Depth_Func::less_equal;

    pass_light = std::make_unique<Pass>("light");
    pass_light->depth_func = Depth_Func::less_equal;

    // Load model, light, etc.
    scene->prepare_scene(scene_name);
}

void Renderer::run()
{
    // Bind key, mouse, resize callback for window.
    std::vector<bool> key_state = std::vector<bool>(1025, false);
    window->setKeyCallback([&](GLFWwindow* glfw_window, int key, int action) mutable {
        if (key >= 0 && key <= 1024) {
            if (action == GLFW_PRESS) key_state[key] = true;
            else if (action == GLFW_RELEASE) key_state[key] = false;
        }
    });

    window->setMouseCallback([&, this](GLFWwindow* glfw_window, uint32_t state, float x, float y, float last_x, float last_y) {
        if (state == Window::MOUSE_RIGHT) {
            float xoffset = x - last_x;
            float yoffset = last_y - y;
            scene->camera->ProcessMouseMovement(xoffset, yoffset);
        }
    });

    window->setResizeCallback([&cam = scene->camera](GLFWwindow* glfw_window, uint32_t width, uint32_t height) {
        cam->cameraWidth = width;
        cam->cameraHeight = height;
    });

    auto process_key = [&key_state, &cam = scene->camera] (float dt) {
        if (key_state[GLFW_KEY_W]) cam->ProcessKeyboard(Camera_Movement::FORWARD, dt);
        if (key_state[GLFW_KEY_S]) cam->ProcessKeyboard(Camera_Movement::BACKWARD, dt);
        if (key_state[GLFW_KEY_A]) cam->ProcessKeyboard(Camera_Movement::LEFT, dt);
        if (key_state[GLFW_KEY_D]) cam->ProcessKeyboard(Camera_Movement::RIGHT, dt);
        if (key_state[GLFW_KEY_E]) cam->ProcessKeyboard(Camera_Movement::UP, dt);
        if (key_state[GLFW_KEY_Q]) cam->ProcessKeyboard(Camera_Movement::DOWN, dt);

        if (key_state[GLFW_KEY_UP]) cam->IncreaseSpeed();
        if (key_state[GLFW_KEY_DOWN]) cam->DecreaseSpeed();
    };

    // Start window mainloop.
    window->mainLoop([&]() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        scene->render(*pass_shade);
        scene->render(*pass_ibl);
        scene->render(*pass_light);

        panel->render();

        process_key(window->deltaTime);
    });

    glfwTerminate();
}
