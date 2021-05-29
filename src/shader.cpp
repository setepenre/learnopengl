#include <fstream>
#include <sstream>

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

#include "error.hpp"
#include "shader.hpp"

static std::pair<std::string, Error> load_shader(const std::string &filepath) {
    std::ifstream in(filepath, std::ios::in);
    if (! in) {
        return {"", wrap("could not open " + filepath)};
    }
    std::ostringstream contents;
    contents << in.rdbuf();
    in.close();
    return {contents.str(), {}};
}

static std::pair<unsigned int, Error> compile_shader(unsigned int type, const std::string &source) {
    unsigned int id = glCreateShader(type);
    const char *src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result = 0;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length = 0;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

        std::vector<char> msg(length);
        glGetShaderInfoLog(id, length, &length, msg.data());

        glDeleteShader(id);
        return {0, wrap(clean(msg.data()))};
    }

    return {id, {}};
}

static std::pair<unsigned int, Error> create_shader(const std::string &vertex, const std::string &fragment) {
    auto [vertex_shader, vertex_error] = compile_shader(GL_VERTEX_SHADER, vertex);
    if (vertex_error.has_value()) {
        return {0, wrap(vertex_error)};
    }
    auto [fragment_shader, fragment_error] = compile_shader(GL_FRAGMENT_SHADER, fragment);
    if (fragment_error.has_value()) {
        return {0, wrap(fragment_error)};
    }

    unsigned int program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    int result = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &result);
    if (result == GL_FALSE) {
        int length = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

        std::vector<char> msg(length);
        glGetProgramInfoLog(program, length, &length, msg.data());

        glDeleteProgram(program);
        return {0, wrap(clean(msg.data()))};
    }

    glValidateProgram(program);

    result = 0;
    glGetProgramiv(program, GL_VALIDATE_STATUS, &result);
    if (result == GL_FALSE) {
        int length = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

        std::vector<char> msg(length);
        glGetProgramInfoLog(program, length, &length, msg.data());

        glDeleteProgram(program);
        return {0, wrap(clean(msg.data()))};
    }

    return {program, {}};
}

Shader::Shader(Shader &&other) : m_ID(other.m_ID), m_location_cache(other.m_location_cache) { other.m_ID = 0; }

Shader &Shader::operator=(Shader &&other) {
    if (&other != this) {
        m_ID       = other.m_ID;
        other.m_ID = 0;

        m_location_cache = other.m_location_cache;
    }
    return *this;
}

Shader::~Shader() {
    if (m_ID) {
        glDeleteProgram(m_ID);
    }
}

void Shader::bind() const { glUseProgram(m_ID); }
void Shader::unbind() const { glUseProgram(0); }

Error Shader::set_uniforms(const std::vector<Uniform> &uniforms) {
    for (auto [name, value] : uniforms) {
        Error error = {};
        if (auto valueptr = std::get_if<int>(&value)) {
            error = set_uniform(name, *valueptr);
        } else if (auto valueptr = std::get_if<float>(&value)) {
            error = set_uniform(name, *valueptr);
        } else if (auto valueptr = std::get_if<f3>(&value)) {
            auto [x, y, z] = *valueptr;
            error          = set_uniform(name, x, y, z);
        } else if (auto valueptr = std::get_if<f4>(&value)) {
            auto [x, y, z, w] = *valueptr;
            error             = set_uniform(name, x, y, z, w);
        } else if (auto valueptr = std::get_if<glm::vec3>(&value)) {
            error = set_uniform(name, *valueptr);
        } else if (auto valueptr = std::get_if<glm::vec4>(&value)) {
            error = set_uniform(name, *valueptr);
        } else if (auto valueptr = std::get_if<glm::mat4>(&value)) {
            error = set_uniform(name, *valueptr);
        } else {
            error = "uniform '" + name + "' type unknown";
        }

        if (error.has_value()) {
            return wrap(error);
        }
    }
    return {};
}

Error Shader::set_uniform(const std::string &name, int value) {
    auto [location, error] = get_uniform_location(name);
    if (error.has_value()) {
        return wrap(error);
    }
    glUniform1i(location, value);
    return {};
}

Error Shader::set_uniform(const std::string &name, float value) {
    auto [location, error] = get_uniform_location(name);
    if (error.has_value()) {
        return wrap(error);
    }
    glUniform1f(location, value);
    return {};
}

Error Shader::set_uniform(const std::string &name, float r, float g, float b) {
    auto [location, error] = get_uniform_location(name);
    if (error.has_value()) {
        return wrap(error);
    }
    glUniform3f(location, r, g, b);
    return {};
}

Error Shader::set_uniform(const std::string &name, float r, float g, float b, float a) {
    auto [location, error] = get_uniform_location(name);
    if (error.has_value()) {
        return wrap(error);
    }
    glUniform4f(location, r, g, b, a);
    return {};
}

Error Shader::set_uniform(const std::string &name, const glm::vec3 &vector) {
    return set_uniform(name, vector.r, vector.g, vector.b);
}

Error Shader::set_uniform(const std::string &name, const glm::vec4 &vector) {
    return set_uniform(name, vector.r, vector.g, vector.b, vector.a);
}

Error Shader::set_uniform(const std::string &name, const glm::mat4 &matrix) {
    auto [location, error] = get_uniform_location(name);
    if (error.has_value()) {
        return wrap(error);
    }
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    return {};
}

std::pair<Shader, Error> Shader::from_files(const std::string &vertex, const std::string &fragment) {
    auto [vertex_shader, vertex_error] = load_shader(vertex);
    if (vertex_error.has_value()) {
        return {Shader(0), wrap(vertex_error)};
    }
    auto [fragment_shader, fragment_error] = load_shader(fragment);
    if (fragment_error.has_value()) {
        return {Shader(0), wrap(fragment_error)};
    }
    auto [shaderID, error] = create_shader(vertex_shader, fragment_shader);
    if (error.has_value()) {
        return {Shader(0), wrap(error)};
    }
    return {Shader(shaderID), {}};
}

Shader::Shader(unsigned int ID) : m_ID(ID) {}

std::pair<int, Error> Shader::get_uniform_location(const std::string &name) {
    if (m_location_cache.contains(name)) {
        return {m_location_cache[name], {}};
    }
    int location = glGetUniformLocation(m_ID, name.c_str());
    if (location == -1) {
        return {location, wrap("glGetUniformLocation: could not find location of '" + name + "'")};
    }
    return {m_location_cache[name] = location, {}};
}
