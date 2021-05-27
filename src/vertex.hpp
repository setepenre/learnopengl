#pragma once

#include <functional>
#include <tuple>
#include <vector>

#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 r {0.0f, 0.0f, 0.0f};
    glm::vec3 color {0.0f, 0.0f, 0.0f};
    glm::vec2 t {0.0f, 0.0f};
};

Vertex operator*(const glm::mat4 &matrix, const Vertex &vertex);

using VertexLayout = std::tuple<unsigned int, unsigned int, unsigned int, unsigned int, const void *>;

std::vector<VertexLayout> vertex_layouts();

using Vertices = std::vector<Vertex>;
Vertices operator+(const Vertices &left, const Vertices &right);
Vertices operator*(const glm::mat4 &matrix, const Vertices &right);
