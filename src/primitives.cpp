#include "primitives.hpp"

#include <glm/gtc/matrix_transform.hpp>

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

Vertices quad(glm::vec3 center, glm::vec3 a, glm::vec3 b, float scale) {
    return quad(center, a, b, scale, {black, black, black, black}, full);
}

Vertices quad(glm::vec3 center, glm::vec3 a, glm::vec3 b, float scale, Color color) {
    return quad(center, a, b, scale, {color, color, color, color}, full);
}

Vertices quad(glm::vec3 center, glm::vec3 a, glm::vec3 b, float scale, const std::array<Color, 4> &colors) {
    return quad(center, a, b, scale, colors, full);
}

Vertices quad(glm::vec3 center, glm::vec3 a, glm::vec3 b, float scale, const std::array<TexCoord, 4> &tex_coords) {
    return quad(center, a, b, scale, {black, black, black, black}, tex_coords);
}

Vertices quad(glm::vec3 center, glm::vec3 a, glm::vec3 b, float scale, const std::array<Color, 4> &colors,
    const std::array<TexCoord, 4> &tex_coords) {
    glm::vec3 normal  = glm::normalize(glm::cross(a, b));
    Vertices vertices = {
        {center + (scale / 2.0f) * (-a + b), colors[0], tex_coords[0], normal},
        {center + (scale / 2.0f) * (-a - b), colors[1], tex_coords[1], normal},
        {center + (scale / 2.0f) * (+a - b), colors[2], tex_coords[2], normal},
        {center + (scale / 2.0f) * (+a + b), colors[3], tex_coords[3], normal},
    };
    return vertices;
}

Vertices cube(glm::vec3 center, glm::vec3 a, glm::vec3 b, glm::vec3 c, float scale) {
    return cube(center, a, b, c, scale, {black, black, black, black}, full);
}

Vertices cube(glm::vec3 center, glm::vec3 a, glm::vec3 b, glm::vec3 c, float scale, Color color) {
    return cube(center, a, b, c, scale, {color, color, color, color}, full);
}

Vertices cube(
    glm::vec3 center, glm::vec3 a, glm::vec3 b, glm::vec3 c, float scale, const std::array<Color, 4> &colors) {
    return cube(center, a, b, c, scale, colors, full);
}

Vertices cube(
    glm::vec3 center, glm::vec3 a, glm::vec3 b, glm::vec3 c, float scale, const std::array<TexCoord, 4> &tex_coords) {
    return cube(center, a, b, c, scale, {black, black, black, black}, tex_coords);
}

Vertices cube(glm::vec3 center, glm::vec3 a, glm::vec3 b, glm::vec3 c, float scale, const std::array<Color, 4> &colors,
    const std::array<TexCoord, 4> &tex_coords) {
    Vertices right      = quad(0.5f * a, b, c, scale, colors, tex_coords);
    Vertices top        = quad(0.5f * b, c, a, scale, colors, tex_coords);
    Vertices front      = quad(0.5f * c, a, b, scale, colors, tex_coords);
    glm::mat4 to_left   = glm::translate(glm::mat4(1.0f), -a);
    glm::mat4 to_bottom = glm::translate(glm::mat4(1.0f), -b);
    glm::mat4 to_back   = glm::translate(glm::mat4(1.0f), -c);

    return glm::translate(glm::mat4(1.0f), center) *
           (right + (to_left * right) + top + (to_bottom * top) + front + (to_back * front));
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
