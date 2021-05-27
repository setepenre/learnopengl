#include <algorithm>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.hpp"
#include "control.hpp"
#include "debug.hpp"
#include "index_buffer.hpp"
#include "primitives.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "vertex_array.hpp"

static float delta_t  = 0.0f;
static float previous = 0.0f;
static Camera camera  = {glm::pi<float>() / 2.0f, {0.0f, 0.0f, 3.0f}, {0.0f, -glm::pi<float>() / 2.0f, 0.0f}};

static Control *controlptr = Control::instance(camera);
static Control &control    = *controlptr;

void viewport_resize(GLFWwindow *, int width, int height) { glViewport(0, 0, width, height); }

constexpr static float w = 2 * glm::pi<float>();
std::tuple<float, float, float> rgb(float t) { return {std::cos(w * t), std::cos(w / 2 * t), std::cos(w / 4 * t)}; }
glm::vec3 orbit(float radius, float t) { return radius * glm::vec3(std::sin(w * t), 0.0f, std::cos(w * t)); }

std::string usage(const std::string &name) {
    return "usage: " + name + "width height\n" + "arguments:\n" +
           "  width     width of window to be created, in pixels\n" +
           "  height    height of window to be created, in pixels\n";
}

std::pair<std::tuple<int, int>, Error> from_args(int argc, char *argv[]) {
    if ((argc - 1) == 2) {
        try {
            return {{std::stoi(argv[1]), std::stoi(argv[2])}, {}};
        } catch (const std::invalid_argument &e) {
            return {{}, wrap(e.what())};
        }
    }
    return {{1280, 720}, {}};
}

Error run(int argc, char *argv[]) {
    auto [args, error] = from_args(argc, argv);
    if (error.has_value()) {
        return wrap(error);
    }
    auto [w, h]            = args;
    const std::string name = "LearnOpenGL";

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHintString(GLFW_X11_CLASS_NAME, ("float_" + name).c_str());
    glfwWindowHintString(GLFW_X11_INSTANCE_NAME, ("float_" + name).c_str());

    GLFWwindow *window = glfwCreateWindow(w, h, name.c_str(), nullptr, nullptr);
    if (! window) {
        const char *error;
        glfwGetError(&error);
        return wrap(error);
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    control.last = 0.5f * glm::vec2(w, h);

    glfwSetFramebufferSizeCallback(window, viewport_resize);
    glfwSetCursorPosCallback(window, Control::mouse);
    glfwSetScrollCallback(window, Control::scroll);

    if (! gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        return wrap("failed to initialize GLAD");
    }

    std::cout << "OpenGL\n"
              << "  Vendor  \t" << glGetString(GL_VENDOR) << "\n"
              << "  Renderer\t" << glGetString(GL_RENDERER) << "\n"
              << "  Version \t" << glGetString(GL_VERSION) << "\n";

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(debug_callback, nullptr);

    glEnable(GL_DEPTH_TEST);

    std::filesystem::path cwd = std::filesystem::path(argv[0]).parent_path();

    glm::vec3 origin = {0.0f, 0.0f, 0.0f};
    glm::vec3 ux {1.0f, 0.0f, 0.0f}, uy {0.0f, 1.0f, 0.0f}, uz {0.0f, 0.0f, 1.0f};
    std::vector<glm::vec3> cubes = {
        {0.0f, 0.0f, 0.0f},
        {2.0f, 5.0f, -15.0f},
        {-1.5f, -2.2f, -2.5f},
        {-3.8f, -2.0f, -12.3f},
        {2.4f, -0.4f, -3.5f},
        {-1.7f, 3.0f, -7.5f},
        {1.3f, -2.0f, -2.5f},
        {1.5f, 2.0f, -2.5f},
        {1.5f, 0.2f, -1.5f},
        {-1.3f, 1.0f, -1.5f},
    };

    Vertices right      = quad({0.5f, 0.0f, 0.0f}, uy, uz, 1.0f);
    Vertices top        = quad({0.0f, 0.5f, 0.0f}, uz, ux, 1.0f);
    Vertices front      = quad({0.0f, 0.0f, 0.5f}, ux, uy, 1.0f);
    glm::mat4 to_left   = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
    glm::mat4 to_bottom = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    glm::mat4 to_back   = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    Vertices cube       = right + (to_left * right) + top + (to_bottom * top) + front + (to_back * front);

    IndexBuffer ib  = {quad_indices(cube)};
    VertexBuffer vb = {std::move(cube)};
    VertexArray va  = {vb};

    glEnable(GL_LINE_SMOOTH);
    Vertices lines        = line(origin, ux, 1.0f, ux) + line(origin, uy, 1.0f, uy) + line(origin, uz, 1.0f, uz);
    IndexBuffer ib_lines  = {line_indices(lines)};
    VertexBuffer vb_lines = {std::move(lines)};
    VertexArray va_lines  = {vb_lines};

    auto [shader, shader_error] = Shader::from_files(cwd / "res/shader.vert", cwd / "res/shader.frag");
    if (shader_error.has_value()) {
        return wrap(shader_error);
    }

    auto [shader_lines, shader_lines_error] = Shader::from_files(cwd / "res/shader.vert", cwd / "res/lines.frag");
    if (shader_lines_error.has_value()) {
        return wrap(shader_lines_error);
    }

    auto [container_texture, container_error] = Texture::from_file(cwd / "res/container.jpg",
        GL_RGB,
        GL_TEXTURE0,
        {
            {GL_TEXTURE_WRAP_S, GL_REPEAT},
            {GL_TEXTURE_WRAP_T, GL_REPEAT},
            {GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR},
            {GL_TEXTURE_MAG_FILTER, GL_LINEAR},
        });
    if (container_error.has_value()) {
        return wrap(container_error);
    }

    auto [awesome_texture, awesome_error] = Texture::from_file(cwd / "res/awesomeface.png",
        GL_RGBA,
        GL_TEXTURE1,
        {
            {GL_TEXTURE_WRAP_S, GL_REPEAT},
            {GL_TEXTURE_WRAP_T, GL_REPEAT},
            {GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR},
            {GL_TEXTURE_MAG_FILTER, GL_LINEAR},
        });
    if (awesome_error.has_value()) {
        return wrap(awesome_error);
    }

    shader.bind();
    if (error = shader.set_uniform1i("u_texture0", container_texture.slot() - GL_TEXTURE0); error.has_value()) {
        return wrap(error);
    }

    if (error = shader.set_uniform1i("u_texture1", awesome_texture.slot() - GL_TEXTURE0); error.has_value()) {
        return wrap(error);
    }

    // float t = 1.0f;
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    while (! glfwWindowShouldClose(window)) {
        control.process_input(window);

        // auto [r, g, b] = rgb(t = ! pause ? 1.0f + glfwGetTime() : t);
        float now = glfwGetTime();
        delta_t   = now - previous;
        previous  = now;

        camera.position(camera.position() + 5.0f * control.movement_direction() * delta_t);
        control.movement_direction({0.0f, 0.0f, 0.0f});

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(camera.fov(), (float) w / (float) h, 0.1f, 100.f);

        container_texture.bind();
        awesome_texture.bind();
        va.bind();
        ib.bind();
        shader.bind();

        if (error = shader.set_uniform1f("u_alpha", control.alpha()); error.has_value()) {
            return wrap(error);
        }

        if (error = shader.set_uniformmat4f("u_view", camera.view()); error.has_value()) {
            return wrap(error);
        }

        if (error = shader.set_uniformmat4f("u_projection", projection); error.has_value()) {
            return wrap(error);
        }

        for (size_t i = 0; i < cubes.size(); ++i) {
            glm::mat4 model = glm::mat4(1.0f);
            model           = glm::translate(model, cubes[i]);
            model           = glm::rotate(model, i * w / 8.0f, glm::vec3(1.0f, 0.3f, 0.5f));

            if (error = shader.set_uniformmat4f("u_model", model); error.has_value()) {
                return wrap(error);
            }
            glDrawElements(GL_TRIANGLES, ib.count(), GL_UNSIGNED_INT, nullptr);
        }

        va_lines.bind();
        ib_lines.bind();
        shader_lines.bind();
        glm::mat4 model = glm::mat4(1.0f);

        if (error = shader_lines.set_uniformmat4f("u_model", model); error.has_value()) {
            return wrap(error);
        }

        if (error = shader_lines.set_uniformmat4f("u_view", camera.view()); error.has_value()) {
            return wrap(error);
        }

        if (error = shader_lines.set_uniformmat4f("u_projection", projection); error.has_value()) {
            return wrap(error);
        }
        glDrawElements(GL_LINES, ib_lines.count(), GL_UNSIGNED_INT, nullptr);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return {};
}

int main(int argc, char *argv[]) {
    glfwInit();

    Error error = run(argc, argv);
    if (error.has_value()) {
        std::cerr << error.value() << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwTerminate();
    return EXIT_SUCCESS;
}
