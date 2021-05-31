#include "control.hpp"

#include <algorithm>
#include <map>

Control *Control::control = nullptr;

constexpr static float pi = glm::pi<float>();

Control::Control(Camera &camera) : m_camera(camera) {}

Control *Control::instance(Camera &camera) {
    if (control == nullptr) {
        control = new Control(camera);
    }
    return control;
}

void Control::process_input(GLFWwindow *window, int key, int, int action, int) {
    assert(control);

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (key == GLFW_KEY_T && action == GLFW_PRESS) {
        int mode = 0;

        glGetIntegerv(GL_POLYGON_MODE, &mode);
        if (mode == GL_LINE) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
    }

    if (key == GLFW_KEY_F && action == GLFW_PRESS) {
        control->m_flashlight = ! control->m_flashlight;
    }

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        control->m_pause = ! control->m_pause;
    }

    if (key == GLFW_KEY_N && action == GLFW_PRESS) {
        control->m_light_count = std::min(control->light_count() + 1, 8);
    }

    if (key == GLFW_KEY_M && action == GLFW_PRESS) {
        control->m_light_count = std::max(control->light_count() - 1, 0);
    }

    glm::vec3 ez = control->m_camera.front(),
              ex = glm::normalize(glm::cross(control->m_camera.front(), control->m_camera.up()));

    static std::map<int, int> actions = {{GLFW_PRESS, 1}, {GLFW_REPEAT, 1}, {GLFW_RELEASE, 0}};
    static std::map<int, int> keys    = {
        {GLFW_KEY_W, 0},
        {GLFW_KEY_S, 1},
        {GLFW_KEY_A, 2},
        {GLFW_KEY_D, 3},
    };

    if (keys.contains(key) && actions.contains(action)) {
        control->m_wsad[keys[key]] = actions[action];
    }

    glm::vec3 acc = (1.0f * control->m_wsad[0] - 1.0f * control->m_wsad[1]) * ez +
                    (-1.0f * control->m_wsad[2] + control->m_wsad[3]) * ex;

    control->m_movement_direction = glm::length(acc) ? glm::normalize(acc) : acc;
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
