#pragma once

#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include <glm/glm.hpp>

#include "error.hpp"

using f3      = std::tuple<float, float, float>;
using f4      = std::tuple<float, float, float, float>;
using Uniform = std::pair<std::string, std::variant<int, float, f3, f4, glm::vec3, glm::mat4>>;

class Shader {
  public:
    Shader(Shader &&other);
    Shader &operator=(Shader &&other);
    ~Shader();

    void bind() const;
    void unbind() const;

    Error set_uniforms(const std::vector<Uniform> &uniforms);

    Error set_uniform(const std::string &name, int);
    Error set_uniform(const std::string &name, float);
    Error set_uniform(const std::string &name, float, float, float);
    Error set_uniform(const std::string &name, float, float, float, float);
    Error set_uniform(const std::string &name, const glm::vec3 &vector);
    Error set_uniform(const std::string &name, const glm::mat4 &matrix);

    static std::pair<Shader, Error> from_files(const std::string &vertex, const std::string &fragment);

    Shader(const Shader &other) = delete;
    Shader &operator=(const Shader &other) = delete;

  private:
    unsigned int m_ID;
    std::unordered_map<std::string, int> m_location_cache;

    Shader(unsigned int ID);

    std::pair<int, Error> get_uniform_location(const std::string &name);
};
