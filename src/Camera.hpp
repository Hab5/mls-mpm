#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <numeric>

enum CameraMove {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

class Camera {
public:
    glm::vec3 position;
    
    glm::vec3 ray00; // Bottom Left of fustrum
    glm::vec3 ray01; // Top Left of fustrum
    glm::vec3 ray10; // Bottom Right of fustrum
    glm::vec3 ray11; // Top Right of fustrum

    Camera(GLFWwindow* window, const glm::vec3 pos);

    void ProcessKeyboard(CameraMove direction, float deltaTime);
    void ProcessMouse(float deltaTime);

    void setSensitivity(float sensitivity);
    void setSpeed(float speed);
    void setFov(float fov);

    inline glm::mat4 GetViewProjection() const {
        return projection * glm::lookAt(position, position + forward, up);
    }

    inline glm::mat4 GetView() const {
        return glm::lookAt(position, position + forward, up);
    }

    inline glm::mat4 GetProjection() const {
        return projection;
    }
    
private:

    GLFWwindow* Window;
    glm::mat4 projection;
    glm::vec3 up;
    glm::vec3 forward;
    glm::vec3 right;

    float yaw;
    float pitch;

    float fieldOfView;
    float mouseSensitivity;
    float movementSpeed;

    float zNear;
    float zFar;

    bool   firstMouseInput;
    double mouseX;
    double mouseY;
    double lastX;
    double lastY;
    
    void updateCamera();
    float getAspectRatio();
};

Camera::Camera(GLFWwindow* window, const glm::vec3 pos)
{
    Window = window;
    firstMouseInput = true;
    position = glm::vec3(pos.x, pos.y, -pos.z);
    yaw = -90.0f;
    pitch = 0.0f;
    fieldOfView = 70.0f;
    mouseSensitivity = 0.08f;
    movementSpeed = 50.0f;

    up = glm::vec3(0.0f, 1.0f, 0.0f);
    forward = glm::vec3(0.0f, 0.0f, -1.0f);
    projection = glm::perspective(fieldOfView, getAspectRatio(), 0.1f, 100000.0f);
}

void Camera::ProcessKeyboard(CameraMove direction, float deltaTime) {
    float velocity = movementSpeed * deltaTime;
    if (direction == FORWARD)
        position += forward * velocity;
    if (direction == BACKWARD)
        position -= forward * velocity;
    if (direction == LEFT)
        position -= right * velocity;
    if (direction == RIGHT)
        position += right * velocity;
    updateCamera();
}

void Camera::ProcessMouse(float deltaTime) {

    glfwGetCursorPos(Window, &mouseX, &mouseY);

    if (firstMouseInput == true) {
        firstMouseInput = false;
        lastX = mouseX;
        lastY = mouseY;
    }

    float xoffset = mouseX - lastX;
    float yoffset = lastY - mouseY;
    
    yaw   += xoffset * mouseSensitivity;
    pitch += yoffset * mouseSensitivity;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    lastX = mouseX;
    lastY = mouseY;
    updateCamera();
}

void Camera::updateCamera() {
    glm::vec3 forwardTmp;
    forwardTmp.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    forwardTmp.y = sin(glm::radians(pitch));
    forwardTmp.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    
    forward = forwardTmp;
    right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
    up = glm::normalize(glm::cross(right, forward));

    // glm::mat4 inv = glm::inverse(GetViewProjection());
    
    // glm::vec4 tmp_ray00 = inv * glm::vec4(-1, -1, 0, 1);
    // glm::vec4 tmp_ray01 = inv * glm::vec4(-1, +1, 0, 1);
    // glm::vec4 tmp_ray10 = inv * glm::vec4(+1, -1, 0, 1);
    // glm::vec4 tmp_ray11 = inv * glm::vec4(+1, +1, 0, 1);
    
    // tmp_ray00 /= tmp_ray00.w;
    // tmp_ray01 /= tmp_ray01.w;
    // tmp_ray10 /= tmp_ray10.w;
    // tmp_ray11 /= tmp_ray11.w;
    
    // tmp_ray00 -= glm::vec4(position, 0.0);
    // tmp_ray01 -= glm::vec4(position, 0.0);
    // tmp_ray10 -= glm::vec4(position, 0.0);
    // tmp_ray11 -= glm::vec4(position, 0.0);

    // ray00 = glm::vec3(tmp_ray00);
    // ray01 = glm::vec3(tmp_ray01);
    // ray10 = glm::vec3(tmp_ray10);
    // ray11 = glm::vec3(tmp_ray11);
    
}

void Camera::setSensitivity(float sensitivity) {
    mouseSensitivity = sensitivity;
}

void Camera::setSpeed(float speed) {
    movementSpeed = speed;
}

void Camera::setFov(float fov) {
    fieldOfView = fov;
    projection = glm::perspective(fieldOfView, getAspectRatio(), 0.1f, 1000.0f);
}
    
float Camera::getAspectRatio() {
    int width, height;
    glfwGetWindowSize(Window, &width, &height);
    return width/height;
}