#include "renderer.hpp"

#include "pass/pass_rect_to_cube.hpp"
#include "pass/pass_ibl_irradiance.hpp"
#include "pass/pass_ibl_prefiltered_map.hpp"
#include "pass/pass_ibl_precomputed_brdf.hpp"

#include "pass/pass_shadow.hpp"
#include "pass/pass_compute_SAT.hpp"

#include "pass/pass_shade.hpp"
#include "pass/pass_skybox.hpp"
#include "pass/pass_light.hpp"

#include <utility>

Renderer::Renderer(const std::string &scene_name)
{
    scene = std::make_shared<Scene>(scene_name);
    window = std::make_unique<Window>(scene->screen_width, scene->screen_height, "RIO: Render In OpenGL");
    panel = std::make_unique<Panel>(window->gl_window, scene);

    // Load model, light, etc.
    scene->prepare_scene(scene_name);
    scene->prepare_present();

    // Initialize all render pass.
    pass_rect_to_cube = std::make_unique<Pass_Rect_To_Cube>("rect_to_cube", scene);
    pass_ibl_irradiance = std::make_unique<Pass_IBL_Irradiance>("ibl_irradiance", scene);
    pass_ibl_prefiltered_map = std::make_unique<Pass_IBL_Prefiltered_Map>("ibl_prefiltered_map", scene);
    pass_ibl_precomputed_brdf = std::make_unique<Pass_IBL_Precomputed_BRDF>("ibl_precomputed_brdf", scene);

    pass_shadow = std::make_unique<Pass_Shadow>("shadow", scene);
    pass_compute_SAT = std::make_unique<Pass_Compute_SAT>("compute_SAT", scene, true);

    pass_shade = std::make_unique<Pass_Shade>("shade", scene);
    pass_skybox = std::make_unique<Pass_Skybox>("skybox", scene);
    pass_skybox->depth_func = Depth_Func::less_equal;
    pass_light = std::make_unique<Pass_Light>("light", scene);
    pass_light->depth_func = Depth_Func::less_equal;
}

void Renderer::run() const
{
    // Bind key, mouse, resize callback for window.
    std::vector<bool> key_state = std::vector<bool>(1025, false);
    bool camera_speed_dirty = false;
    window->set_key_callback([&](GLFWwindow* glfw_window, int key, int action) mutable {
        if (key == GLFW_KEY_UP || key == GLFW_KEY_DOWN) {
            if (action == GLFW_PRESS) {
                key_state[key] = true;
            } else if (action == GLFW_RELEASE) {
                key_state[key] = false;
                camera_speed_dirty = false;
            }

            return;
        }

        if (key >= 0 && key <= 1024) {
            if (action == GLFW_PRESS) {
                key_state[key] = true;
            } else if (action == GLFW_RELEASE) {
                key_state[key] = false;
            }
        }

        if (key == GLFW_KEY_F && action == GLFW_PRESS) {
            window->switch_full_screen();
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

    window->set_mouse_callback([&](GLFWwindow* glfw_window, uint32_t state, float x, float y, float last_x, float last_y) {
        if (state == Window::MOUSE_RIGHT) {
            float xoffset = x - last_x;
            float yoffset = last_y - y;
            scene->camera->process_mouse_movement(xoffset, yoffset);
        }
    });

    window->set_resize_callback([&](GLFWwindow* glfw_window, uint32_t width, uint32_t height) {
        *window->window_width = width;
        *window->window_height = height;
    });

    auto process_key = [&] (float dt) {
        auto& cam = scene->camera;
        if (key_state[GLFW_KEY_W]) cam->process_keyboard(Camera_Movement::FORWARD, dt);
        if (key_state[GLFW_KEY_S]) cam->process_keyboard(Camera_Movement::BACKWARD, dt);
        if (key_state[GLFW_KEY_A]) cam->process_keyboard(Camera_Movement::LEFT, dt);
        if (key_state[GLFW_KEY_D]) cam->process_keyboard(Camera_Movement::RIGHT, dt);
        if (key_state[GLFW_KEY_E]) cam->process_keyboard(Camera_Movement::UP, dt);
        if (key_state[GLFW_KEY_Q]) cam->process_keyboard(Camera_Movement::DOWN, dt);

        if (camera_speed_dirty == false) {
            if (key_state[GLFW_KEY_UP]) {
                cam->increase_speed();
                camera_speed_dirty = true;
            }

            if (key_state[GLFW_KEY_DOWN]) {
                cam->decrease_speed();
                camera_speed_dirty = true;
            }
        }
    };

    // Start window mainloop.
    window->main_loop([&]() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Pass precompute.
        pass_rect_to_cube->render();
        pass_ibl_irradiance->render();
        pass_ibl_prefiltered_map->render();
        pass_ibl_precomputed_brdf->render();

        // Pass runtime.
        pass_shadow->render();
        pass_compute_SAT->render();

        pass_shade->render();
        pass_skybox->render();
        pass_light->render();

        // Pass ui.
        panel->render();

        process_key(window->delta_time);
    });

    window->exit();
}
