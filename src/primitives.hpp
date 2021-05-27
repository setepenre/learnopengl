#pragma once

#include <vector>

#include "vertex.hpp"

using TexCoord = glm::vec2;
using Color    = glm::vec3;
Vertices line(glm::vec3 origin, glm::vec3 direction, float length, Color color = {0.0f, 0.0f, 0.0f});
Vertices quad(glm::vec3 center, glm::vec3 a, glm::vec3 b, float size);
Vertices quad(glm::vec3 center, glm::vec3 a, glm::vec3 b, float size, Color color);
Vertices quad(glm::vec3 center, glm::vec3 a, glm::vec3 b, float size, const std::array<Color, 4> &colors);
Vertices quad(glm::vec3 center, glm::vec3 a, glm::vec3 b, float size, const std::array<TexCoord, 4> &tex_coords);
Vertices quad(glm::vec3 center, glm::vec3 a, glm::vec3 b, float size, const std::array<Color, 4> &colors,
    const std::array<TexCoord, 4> &tex_coords);

using Indices = std::vector<unsigned int>;
Indices line_indices(const Vertices &vertices);
Indices quad_indices(const Vertices &vertices);
