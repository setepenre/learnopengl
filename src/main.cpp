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
    const std::optional<std::vector<std::pair<std::string, Texture>>> &textures = {},
    const std::vector<Light> &lights                                            = {}) {
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

    if (Error error = shader.set_lights("u_lights", lights); error.has_value()) {
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
    glfwSetKeyCallback(window, Control::process_input);
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

    std::vector<glm::vec3> cube_positions = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f, 2.0f, -2.5f),
        glm::vec3(1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f),
    };

    Vertices cube_vertices = cube(cube_positions[0], ux, uy, uz, 1.0f);
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

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    while (! glfwWindowShouldClose(window)) {
        float now = glfwGetTime();
        delta_t   = now - previous;
        previous  = now;

        camera.position(camera.position() + 5.0f * control.movement_direction() * delta_t);
        glm::mat4 projection = glm::perspective(camera.fov(), (float) w / (float) h, 0.1f, 100.f);

        glm::vec3 white                                          = glm::vec3(1.0f);
        std::vector<std::pair<glm::vec3, glm::vec3>> lights_data = {
            {{0.7f, 0.2f, 2.0f}, white},
            {{2.3f, -3.3f, -4.0f}, glm::vec3(1.0f, 0.0f, 0.0f)},
            {{-4.0f, 2.0f, -12.0f}, glm::vec3(0.0f, 1.0f, 0.0f)},
            {{0.0f, 0.0f, -3.0f}, glm::vec3(0.0f, 0.0f, 1.0f)},
        };

        Light flashlight = {
            glm::vec4(camera.position(), 1.0f),
            true,
            camera.front(),
            std::cos(glm::radians(12.5f)),
            std::cos(glm::radians(17.5f)),
            0.0f * white,
            0.5f * white,
            white,
            1.0f,
            0.09f,
            0.032f,
        };

        int nlights               = std::min(control.light_count(), (int) lights_data.size());
        std::vector<Light> lights = {};
        std::vector<std::pair<glm::vec3, glm::vec3>> visible_lights = {};
        for (int i = 0; i < nlights; ++i) {
            auto [light_position, light_color] = lights_data[i];
            lights.push_back({
                .position  = glm::vec4(light_position, 1.0f),
                .ambient   = 0.2f * light_color,
                .diffuse   = 0.5f * light_color,
                .specular  = light_color,
                .linear    = 0.09f,
                .quadratic = 0.032f,
            });
            visible_lights.push_back({light_position, light_color});
        };
        if (control.flashlight()) {
            lights.push_back(flashlight);
            nlights += 1;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (size_t i = 0; i < cube_positions.size(); ++i) {
            const auto &cube_position = cube_positions[i];
            glm::mat4 model =
                glm::rotate(glm::translate(glm::mat4(1.0f), cube_position), i * pi / 8.0f, glm::vec3(1.0f, 0.3f, 0.5f));
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

                        {"u_material.color", glm::vec4(1.0f, 0.5f, 0.0f, 0.0f)},
                        {"u_material.shininess", 64.0f},

                        {"u_nlights", std::min(nlights, 8)},
                    },
                    {{
                        {"u_material.diffuse", texture_container},
                        {"u_material.specular", texture_specular},
                    }},
                    lights);
                error.has_value()) {
                return wrap(error);
            }
        }

        for (auto [light_position, light_color] : visible_lights) {
            if (error = draw(Primitive::TRIANGLES,
                    va_lights,
                    ib,
                    shader_light,
                    {
                        {"u_model", glm::scale(glm::translate(glm::mat4(1.0f), light_position), glm::vec3(0.2f))},
                        {"u_view", camera.view()},
                        {"u_projection", projection},
                        {"u_light_color", light_color},
                    });
                error.has_value()) {
                return wrap(error);
            }
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
