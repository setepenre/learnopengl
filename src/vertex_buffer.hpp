#pragma once

#include <glad/glad.h>

#include <vector>

#include "vertex.hpp"

class VertexBuffer {
  public:
    VertexBuffer(std::vector<Vertex> &&vertices);
    ~VertexBuffer();

    void bind() const;
    void unbind() const;

  private:
    unsigned int m_ID;
};
