#include "renderer.hpp"

#include "io/loader.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <utility>

Renderer::Renderer() :
    window_render(SCR_WIDTH, SCR_HEIGHT, "RIO: Render In OpenGL"),
    scene(), pass()
{
    pass.shaderInit();

    scene.camera.cameraWidth = window_render.screenWidth;
    scene.camera.cameraHeight = window_render.screenHeight;

    load_model_to_scene(scene, FileSystem::getPath("runtime/assets/models/nanosuit/nanosuit.obj"));
}

void Renderer::run()
{
    // Bind key, mouse, resize callback for window.
    std::vector<bool> key_state = std::vector<bool>(1025, false);
    window_render.setKeyCallback([&](GLFWwindow* glfw_window, int key, int action) mutable {
        if (key >= 0 && key <= 1024) {
            if (action == GLFW_PRESS) key_state[key] = true;
            else if (action == GLFW_RELEASE) key_state[key] = false;
        }
    });

    window_render.setMouseCallback([&, this](GLFWwindow* glfw_window, uint32_t state, float x, float y, float last_x, float last_y) {
        if (state == Window::MOUSE_RIGHT) {
            float xoffset = x - last_x;
            float yoffset = last_y - y;
            scene.camera.ProcessMouseMovement(xoffset, yoffset);
        }
    });

    window_render.setResizeCallback([&cam = scene.camera](GLFWwindow* glfw_window, uint32_t width, uint32_t height) {
        cam.cameraWidth = width;
        cam.cameraHeight = height;
    });

    auto process_key = [&key_state, &cam = scene.camera] (float dt) {
        if (key_state[GLFW_KEY_W]) cam.ProcessKeyboard(Camera_Movement::FORWARD, dt);
        if (key_state[GLFW_KEY_S]) cam.ProcessKeyboard(Camera_Movement::BACKWARD, dt);
        if (key_state[GLFW_KEY_A]) cam.ProcessKeyboard(Camera_Movement::LEFT, dt);
        if (key_state[GLFW_KEY_D]) cam.ProcessKeyboard(Camera_Movement::RIGHT, dt);
        if (key_state[GLFW_KEY_E]) cam.ProcessKeyboard(Camera_Movement::UP, dt);
        if (key_state[GLFW_KEY_Q]) cam.ProcessKeyboard(Camera_Movement::DOWN, dt);

        if (key_state[GLFW_KEY_UP]) cam.IncreaseSpeed();
        if (key_state[GLFW_KEY_DOWN]) cam.DecreaseSpeed();
    };

    // Start window mainloop.
    window_render.mainLoop([&]() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        scene.render(pass);

        process_key(window_render.deltaTime);
    });

    glfwTerminate();
}
