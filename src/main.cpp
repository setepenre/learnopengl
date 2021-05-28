#include <algorithm>
#include <cmath>
#include <filesystem>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
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

constexpr static float pi = glm::pi<float>();

static float delta_t  = 0.0f;
static float previous = 0.0f;
static Camera camera  = {pi / 2.0f, {1.0f, 3.0f, 1.0f}, {-pi / 3.0f, 3.0f * -pi / 4.0f, 0.0f}};

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

enum Primitive { LINES = GL_LINES, TRIANGLES = GL_TRIANGLES };
Error draw(Primitive primitive, const VertexArray &va, const IndexBuffer &ib, Shader &shader,
    const std::vector<Uniform> &uniforms,
    const std::optional<std::vector<std::pair<std::string, Texture>>> &textures = {}) {
    va.bind();
    ib.bind();
    shader.bind();

    if (textures.has_value()) {
        std::vector<Uniform> samplers = {};
        for (const auto &[name, texture] : textures.value()) {
            texture.bind();
            samplers.push_back({name, texture.slot()});
        }

        if (Error error = shader.set_uniforms(samplers); error.has_value()) {
            return error;
        }
    }

    if (Error error = shader.set_uniforms(uniforms); error.has_value()) {
        return error;
    }

    glDrawElements(primitive, ib.count(), GL_UNSIGNED_INT, nullptr);
    return {};
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
    glm::vec3 cube_position = {0.0f, 0.0f, 0.0f};

    Vertices cube_vertices = cube(cube_position, ux, uy, uz, 1.0f);
    IndexBuffer ib         = {quad_indices(cube_vertices)};
    VertexBuffer vb        = {std::move(cube_vertices)};
    VertexArray va         = {vb};
    VertexArray va_lights  = {vb};

    glEnable(GL_LINE_SMOOTH);
    Vertices lines        = line(origin, ux, 1.0f, ux) + line(origin, uy, 1.0f, uy) + line(origin, uz, 1.0f, uz);
    IndexBuffer ib_lines  = {line_indices(lines)};
    VertexBuffer vb_lines = {std::move(lines)};
    VertexArray va_lines  = {vb_lines};

    auto [shader, shader_error] = Shader::from_files(cwd / "res/shader.vert", cwd / "res/shader.frag");
    if (shader_error.has_value()) {
        return wrap(shader_error);
    }

    auto [shader_light, shader_light_error] = Shader::from_files(cwd / "res/shader.vert", cwd / "res/light.frag");
    if (shader_light_error.has_value()) {
        return wrap(shader_light_error);
    }

    auto [shader_lines, shader_lines_error] = Shader::from_files(cwd / "res/shader.vert", cwd / "res/lines.frag");
    if (shader_lines_error.has_value()) {
        return wrap(shader_lines_error);
    }

    auto [texture_container, texture_container_error] = Texture::from_file(cwd / "res/woodcontainer_steelborder.png",
        GL_TEXTURE0,
        {
            {GL_TEXTURE_WRAP_S, GL_REPEAT},
            {GL_TEXTURE_WRAP_T, GL_REPEAT},
            {GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR},
            {GL_TEXTURE_MAG_FILTER, GL_LINEAR},
        });
    if (texture_container_error.has_value()) {
        return wrap(texture_container_error);
    }

    auto [texture_specular, texture_specular_error] =
        Texture::from_file(cwd / "res/woodcontainer_steelborder_specular.png",
            GL_TEXTURE1,
            {
                {GL_TEXTURE_WRAP_S, GL_REPEAT},
                {GL_TEXTURE_WRAP_T, GL_REPEAT},
                {GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR},
                {GL_TEXTURE_MAG_FILTER, GL_LINEAR},
            });
    if (texture_specular_error.has_value()) {
        return wrap(texture_specular_error);
    }

    auto [texture_emissive, texture_emissive_error] = Texture::from_file(cwd / "res/matrix.jpg",
        GL_TEXTURE2,
        {
            {GL_TEXTURE_WRAP_S, GL_REPEAT},
            {GL_TEXTURE_WRAP_T, GL_REPEAT},
            {GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR},
            {GL_TEXTURE_MAG_FILTER, GL_LINEAR},
        });
    if (texture_emissive_error.has_value()) {
        return wrap(texture_emissive_error);
    }

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    while (! glfwWindowShouldClose(window)) {
        control.process_input(window);

        float now = glfwGetTime();
        delta_t   = now - previous;
        previous  = now;

        camera.position(camera.position() + 5.0f * control.movement_direction() * delta_t);
        control.movement_direction({0.0f, 0.0f, 0.0f});
        glm::mat4 projection = glm::perspective(camera.fov(), (float) w / (float) h, 0.1f, 100.f);

        glm::vec3 light_position {orbit(3.0f, 0.1f * now)}, light_color {1.0f, 1.0f, 1.0f};

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 model = glm::translate(glm::mat4(1.0f), cube_position);
        if (error = draw(Primitive::TRIANGLES,
                va,
                ib,
                shader,
                {
                    {"u_model", model},
                    {"u_ti_model", glm::transpose(glm::inverse(model))},
                    {"u_view", camera.view()},
                    {"u_projection", projection},
                    {"u_view_position", camera.position()},

                    {"u_material.shininess", 32.0f},

                    {"u_light.position", light_position},
                    {"u_light.ambient", 0.2f * light_color},
                    {"u_light.diffuse", 0.5f * light_color},
                    {"u_light.specular", light_color},

                    {"u_tex_t_offset", 0.1f * now},
                },
                {{
                    {"u_material.diffuse", texture_container},
                    {"u_material.specular", texture_specular},
                    {"u_material.emissive", texture_emissive},
                }});
            error.has_value()) {
            return wrap(error);
        }

        if (error = draw(Primitive::TRIANGLES,
                va_lights,
                ib,
                shader_light,
                {
                    {"u_model", glm::scale(glm::translate(glm::mat4(1.0f), light_position), glm::vec3(0.2f))},
                    {"u_view", camera.view()},
                    {"u_projection", projection},
                    {"u_light_color", light_color},
                    {"u_object_color", light_color},
                });
            error.has_value()) {
            return wrap(error);
        }

        if (error = draw(Primitive::LINES,
                va_lines,
                ib_lines,
                shader_lines,
                {
                    {"u_model", glm::mat4(1.0f)},
                    {"u_view", camera.view()},
                    {"u_projection", projection},
                });
            error.has_value()) {
            return wrap(error);
        }

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
