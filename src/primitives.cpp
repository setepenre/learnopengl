#include "primitives.hpp"
#include <iostream>

Vertices line(glm::vec3 origin, glm::vec3 direction, float length, glm::vec3 color) {
    auto left  = origin;
    auto right = origin + length * direction;
    return {
        {left, color},
        {right, color},
    };
}

constexpr static TexCoord zero                = {0.0f, 0.0f};
constexpr static std::array<TexCoord, 4> full = {zero, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}};

constexpr static Color black = {0.0f, 0.0f, 0.0f};

Vertices quad(glm::vec3 center, glm::vec3 a, glm::vec3 b, float size) {
    return quad(center, a, b, size, {black, black, black, black}, full);
}

Vertices quad(glm::vec3 center, glm::vec3 a, glm::vec3 b, float size, Color color) {
    return quad(center, a, b, size, {color, color, color, color}, full);
}

Vertices quad(glm::vec3 center, glm::vec3 a, glm::vec3 b, float size, const std::array<Color, 4> &colors) {
    return quad(center, a, b, size, colors, full);
}

Vertices quad(glm::vec3 center, glm::vec3 a, glm::vec3 b, float size, const std::array<TexCoord, 4> &tex_coords) {
    return quad(center, a, b, size, {black, black, black, black}, tex_coords);
}

Vertices quad(glm::vec3 center, glm::vec3 a, glm::vec3 b, float size, const std::array<Color, 4> &colors,
    const std::array<TexCoord, 4> &tex_coords) {
    Vertices vertices = {
        {center + (size / 2.0f) * (+a + b), colors[0], tex_coords[0]},
        {center + (size / 2.0f) * (-a + b), colors[1], tex_coords[1]},
        {center + (size / 2.0f) * (-a - b), colors[2], tex_coords[2]},
        {center + (size / 2.0f) * (+a - b), colors[3], tex_coords[3]},
    };
    return vertices;
}

Indices line_indices(const Vertices &vertices) {
    Indices indices;
    indices.reserve(vertices.size() % 2 ? (vertices.size() > 0 ? vertices.size() - 1 : 0) : vertices.size());
    for (size_t i = 0; i < indices.capacity(); ++i) {
        indices.push_back(i);
    }
    return indices;
}

Indices quad_indices(const Vertices &vertices) {
    Indices indices, quad {0, 1, 2, 2, 3, 0};
    const size_t n = std::floor(vertices.size() / 4);
    indices.reserve(quad.size() * n);
    for (size_t offset = 0; offset < n; ++offset) {
        for (auto i : quad) {
            indices.push_back(offset * 4 + i);
        }
    }
    return indices;
}
