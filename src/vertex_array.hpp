#pragma once

#include "vertex_buffer.hpp"

class VertexArray {
  public:
    VertexArray(const VertexBuffer &vb);
    ~VertexArray();

    void bind() const;
    void unbind() const;

  private:
    unsigned int m_ID;
};
