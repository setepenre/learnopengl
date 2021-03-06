#pragma once

#include <array>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "camera.hpp"

class Control {
  public:
    Control(Control &) = delete;

    void operator=(const Control &) = delete;

    static Control *instance(Camera &camera);

    static void process_input(GLFWwindow *window, int key, int scancode, int action, int mods);
    static void mouse(GLFWwindow *window, double pos_x, double pos_y);
    static void scroll(GLFWwindow *window, double offset_x, double offset_y);

    glm::vec2 last {0.0f, 0.0f};

    void movement_direction(glm::vec3 direction) { m_movement_direction = direction; }

    bool pause() { return m_pause; }
    bool flashlight() { return m_flashlight; }
    int light_count() { return m_light_count; }
    const glm::vec3 &movement_direction() { return m_movement_direction; }

  protected:
    Control(Camera &camera);

    static Control *control;

  private:
    bool m_pause {false};
    bool m_flashlight {false};
    bool m_first_mouse_event {true};

    int m_light_count {0};
    std::array<int, 4> m_wsad {0, 0, 0, 0};
    glm::vec3 m_movement_direction {0.0f, 0.0f, 0.0f};

    Camera &m_camera;
};
