#include "vertex.hpp"

#include <cstddef>

#include <glad/glad.h>

Vertex operator*(const glm::mat4 &matrix, const Vertex &vertex) {
    return {matrix * glm::vec4(vertex.r, 1.0f), vertex.color, vertex.t};
}

std::vector<VertexLayout> vertex_layouts() {
    VertexLayout position  = {3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *) offsetof(Vertex, r)};
    VertexLayout color     = {3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *) offsetof(Vertex, color)};
    VertexLayout tex_coord = {2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *) offsetof(Vertex, t)};
    return {{position, color, tex_coord}};
}

Vertices operator+(const Vertices &left, const Vertices &right) {
    Vertices acc;
    acc.reserve(left.size() + right.size());
    std::copy(left.begin(), left.end(), std::back_inserter(acc));
    std::copy(right.begin(), right.end(), std::back_inserter(acc));
    return acc;
}

Vertices operator*(const glm::mat4 &matrix, const Vertices &vertices) {
    Vertices new_vertices;
    new_vertices.reserve(vertices.size());
    for (const auto &vertex : vertices) {
        new_vertices.push_back(matrix * vertex);
    }
    return new_vertices;
}
