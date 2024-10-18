#include "renderer.hpp"

#include <utility>

Renderer::Renderer(std::string scene_name)
{
    scene = std::make_shared<Scene>(scene_name);

    auto &camera = scene->camera;
    window = std::make_unique<Window>(camera->cameraWidth, camera->cameraHeight, "RIO: Render In OpenGL");
    panel = std::make_unique<Panel>(window->glWindow, scene);

    // Initialize all render pass.
    pass_shadow = std::make_unique<Pass>("shadow");
    pass_shade = std::make_unique<Pass>("shade");

    pass_rect_to_cube = std::make_unique<Pass>("rect_to_cube");
    pass_skybox = std::make_unique<Pass>("skybox");
    pass_skybox->depth_func = Depth_Func::less_equal;

    pass_ibl_irradiance = std::make_unique<Pass>("ibl_irradiance");
    pass_ibl_prefiltered_map = std::make_unique<Pass>("ibl_prefiltered_map");
    pass_ibl_precomputed_brdf = std::make_unique<Pass>("ibl_precomputed_brdf");

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
            if (action == GLFW_PRESS) {
                key_state[key] = true;
            } else if (action == GLFW_RELEASE) {
                key_state[key] = false;
            }
        }

        if (key == GLFW_KEY_F && action == GLFW_PRESS) {
            window->switchFullScreen();
        }

        if (key == GLFW_KEY_M && action == GLFW_PRESS) {
            scene->scene_config->render_gui = !scene->scene_config->render_gui;
        }

        if (key == GLFW_KEY_P && action == GLFW_PRESS) {
            scene->save_output();
        }

        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            window->exit();
        }
    });

    window->setMouseCallback([&](GLFWwindow* glfw_window, uint32_t state, float x, float y, float last_x, float last_y) {
        if (state == Window::MOUSE_RIGHT) {
            float xoffset = x - last_x;
            float yoffset = last_y - y;
            scene->camera->ProcessMouseMovement(xoffset, yoffset);
        }
    });

    window->setResizeCallback([&](GLFWwindow* glfw_window, uint32_t width, uint32_t height) {
        auto& cam = scene->camera;
        cam->cameraWidth = width;
        cam->cameraHeight = height;
        scene->screen_width = width;
        scene->screen_height = height;
    });

    auto process_key = [&] (float dt) {
        auto& cam = scene->camera;
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

        // Pass precompute.
        scene->render(*pass_rect_to_cube);
        scene->render(*pass_ibl_irradiance);
        scene->render(*pass_ibl_prefiltered_map);
        scene->render(*pass_ibl_precomputed_brdf);

        // Pass runtime.
        scene->render(*pass_skybox);
        scene->render(*pass_shadow);
        scene->render(*pass_shade);
        scene->render(*pass_light);

        // Pass ui.
        panel->render();

        process_key(window->deltaTime);
    });

    glfwTerminate();
}
