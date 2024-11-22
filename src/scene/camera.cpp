#include "camera.hpp"

#include <iostream>

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : cam_front(glm::vec3(0.0f, 0.0f, -1.0f)),
      movement_speed(SPEED), mouse_sensitivity(SENSITIVITY), zoom(ZOOM)
{
    cam_position = position;
    cam_world_up = up;
    cam_yaw = yaw;
    cam_pitch = pitch;
    update_camera_vectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : cam_front(
            glm::vec3(0.0f, 0.0f, -1.0f)), movement_speed(SPEED), mouse_sensitivity(SENSITIVITY), zoom(ZOOM)
{
    cam_position = glm::vec3(posX, posY, posZ);
    cam_world_up = glm::vec3(upX, upY, upZ);
    cam_yaw = yaw;
    cam_pitch = pitch;
    update_camera_vectors();
}

// returns the view matrix calculated using Euler Angles and the LookAt Matrix
glm::mat4 Camera::get_view_matrix() const
{
    return glm::lookAt(cam_position, cam_position + cam_front, cam_up);
}

glm::mat4 Camera::get_projection_matrix() const
{
    return glm::perspective(glm::radians(zoom), (float)camera_width / (float)camera_height, z_near, z_far);
}

void Camera::process_keyboard(Camera_Movement direction, float delta_time)
{
    float velocity = movement_speed * delta_time;

    if (direction == FORWARD) {
        cam_position += cam_front * velocity;
    }

    if (direction == BACKWARD) {
        cam_position -= cam_front * velocity;
    }

    if (direction == LEFT) {
        cam_position -= cam_right * velocity;
    }

    if (direction == RIGHT) {
        cam_position += cam_right * velocity;
    }

    if (direction == UP) {
        cam_position += cam_world_up * velocity;
    }

    if (direction == DOWN) {
        cam_position -= cam_world_up * velocity;
    }
}

void Camera::increase_speed(float deltaSpeed)
{
    movement_speed *= 1.1f;
}

void Camera::decrease_speed(float deltaSpeed)
{
    if (movement_speed > deltaSpeed * 1.1f) {
        movement_speed /= 1.1f;
    } else {
        std::cout << "Camera speed reached minimum limit." << std::endl;
    }
}

void Camera::process_mouse_movement(float xoffset, float yoffset, GLboolean constrain_pitch)
{
    xoffset *= mouse_sensitivity;
    yoffset *= mouse_sensitivity;

    cam_yaw += xoffset;
    cam_pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrain_pitch) {
        if (cam_pitch > 89.0f) {
            cam_pitch = 89.0f;
        } else if (cam_pitch < -89.0f) {
            cam_pitch = -89.0f;
        }
    }

    // update Front, Right and Up Vectors using the updated Euler angles
    update_camera_vectors();
}

void Camera::process_mouse_scroll(float y_offset)
{
    zoom -= (float) y_offset;
    if (zoom < 1.0f)
        zoom = 1.0f;
    if (zoom > 45.0f)
        zoom = 45.0f;
}

void Camera::update_camera_vectors()
{
    // Calculate the new Front vector.
    glm::vec3 front;
    front.x = cos(glm::radians(cam_yaw)) * cos(glm::radians(cam_pitch));
    front.y = sin(glm::radians(cam_pitch));
    front.z = sin(glm::radians(cam_yaw)) * cos(glm::radians(cam_pitch));
    cam_front = glm::normalize(front);

    // Also re-calculate the Right and Up vector.
    // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    cam_right = glm::normalize(glm::cross(cam_front, cam_world_up));
    cam_up = glm::normalize(glm::cross(cam_right, cam_front));
}

void Camera::reset_camera()
{
    cam_position = glm::vec3{0.0, 0.0, 5.0};
    cam_yaw = -90.0f;
    cam_pitch = 0.0f;

    // Update camera matrix after reset.
    update_camera_vectors();
}
