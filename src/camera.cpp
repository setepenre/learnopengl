#include "camera.hpp"

#include <algorithm>

#include <glm/gtc/constants.hpp>

static glm::vec3 front_from_euler(glm::vec3 euler) {
    return glm::normalize(
        glm::vec3(std::cos(euler.x) * std::cos(euler.y), std::sin(euler.x), std::cos(euler.x) * std::sin(euler.y)));
}

static glm::vec3 up_from_euler(glm::vec3) {
    // return glm::normalize(
    //     glm::vec3(std::sin(euler.z), std::cos(euler.x) * std::cos(euler.z), std::sin(euler.x) * std::cos(euler.z)));
    return {0.0f, 1.0f, 0.0f};
}

Camera::Camera(float fov, glm::vec3 position, glm::vec3 euler) : m_fov(fov), m_position(position), m_euler(euler) {
    m_front = front_from_euler(m_euler);
    m_up    = up_from_euler(m_euler);
}

void Camera::euler(glm::vec3 euler) {
    m_euler   = euler;
    m_euler.x = std::clamp(m_euler.x, -0.99f * glm::pi<float>() / 2.0f, +0.99f * glm::pi<float>() / 2.0f);

    m_front = front_from_euler(m_euler);
    m_up    = up_from_euler(m_euler);
}
