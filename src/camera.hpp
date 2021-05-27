#pragma once

#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
  public:
    Camera(float fov, glm::vec3 position, glm::vec3 euler);

    void fov(float fov) { m_fov = fov; }
    void position(glm::vec3 position) { m_position = position; }
    void euler(glm::vec3 euler);

    float fov() const { return m_fov; }

    const glm::vec3 &position() const { return m_position; }
    const glm::vec3 &euler() const { return m_euler; }
    const glm::vec3 &front() const { return m_front; }
    const glm::vec3 &up() const { return m_up; }

    const glm::mat4 view() const { return glm::lookAt(m_position, m_position + m_front, m_up); }

  private:
    float m_fov;

    glm::vec3 m_position;
    glm::vec3 m_euler;
    glm::vec3 m_front;
    glm::vec3 m_up;
};
