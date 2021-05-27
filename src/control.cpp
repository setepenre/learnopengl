#include "control.hpp"

#include <algorithm>

Control *Control::control = nullptr;

constexpr static float pi = glm::pi<float>();

Control::Control(Camera &camera) : m_camera(camera) {}

Control *Control::instance(Camera &camera) {
    if (control == nullptr) {
        control = new Control(camera);
    }
    return control;
}

void Control::process_input(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
        int mode = 0;

        glGetIntegerv(GL_POLYGON_MODE, &mode);
        if (mode == GL_LINE) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
        m_pause = ! m_pause;
    }

    glm::vec3 acc = {0.0f, 0.0f, 0.0f};
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        acc += m_camera.front();
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        acc -= m_camera.front();
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        acc -= glm::normalize(glm::cross(m_camera.front(), m_camera.up()));
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        acc += glm::normalize(glm::cross(m_camera.front(), m_camera.up()));
    }

    m_movement_direction = glm::length(acc) ? glm::normalize(acc) : acc;
}

void Control::mouse(GLFWwindow *, double pos_x, double pos_y) {
    assert(control);

    if (control->m_first_mouse_event) {
        control->last                = {pos_x, pos_y};
        control->m_first_mouse_event = false;
    }

    constexpr glm::mat2 r = glm::mat2(0.0f, -1.0f, 1.0f, 0.0f);
    control->m_camera.euler(
        control->m_camera.euler() - 0.002f * glm::vec3(r * (glm::vec2(pos_x, pos_y) - control->last), 0.0f));
    control->last = {pos_x, pos_y};
}

void Control::scroll(GLFWwindow *, double, double offset_y) {
    assert(control);

    control->m_camera.fov(std::clamp(control->m_camera.fov() - 0.1f * (float) offset_y, +0.01f * pi / 2.0f, pi / 2.0f));
}
