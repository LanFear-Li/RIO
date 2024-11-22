#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>

enum Camera_Movement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

// Default camera values.
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera final
{
public:
    Camera() = delete;
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           float yaw = YAW, float pitch = PITCH);

    glm::mat4 get_view_matrix() const;
    glm::mat4 get_projection_matrix() const;

    void increase_speed(float deltaSpeed = 1.0f);
    void decrease_speed(float deltaSpeed = 1.0f);

    void process_keyboard(Camera_Movement direction, float delta_time);
    void process_mouse_movement(float xoffset, float yoffset, GLboolean constrain_pitch = true);
    void process_mouse_scroll(float y_offset);

    // Calculates the front vector from the Camera's (updated) Euler Angles.
    void update_camera_vectors();
    void reset_camera();

    // Camera attributes.
    glm::vec3 cam_position{};
    glm::vec3 cam_front;
    glm::vec3 cam_up{};
    glm::vec3 cam_right{};
    glm::vec3 cam_world_up{};

    float cam_yaw;
    float cam_pitch;

    float z_near = 0.1f;
    float z_far = 1000.0f;

    std::shared_ptr<uint32_t> camera_width;
    std::shared_ptr<uint32_t> camera_height;

    // Camera options.
    float movement_speed;
    float mouse_sensitivity;
    float zoom;
};
