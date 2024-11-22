#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
    // Constructor with vectors.
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           float yaw = YAW, float pitch = PITCH);
    // Constructor with scalar values.
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

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

    uint32_t camera_width = 1000;
    uint32_t camera_height = 800;

    // Camera options.
    float movement_speed;
    float mouse_sensitivity;
    float zoom;
};
