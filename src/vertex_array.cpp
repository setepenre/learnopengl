#include "vertex_array.hpp"
#include "vertex.hpp"

#include <glad/glad.h>

VertexArray::VertexArray(const VertexBuffer &vb) {
    glGenVertexArrays(1, &m_ID);
    glBindVertexArray(m_ID);

    vb.bind();
    auto layouts = vertex_layouts();
    for (size_t i = 0; i < layouts.size(); ++i) {
        auto [count, type, normalized, stride, offset] = layouts[i];
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, count, type, normalized, stride, offset);
    }
}

VertexArray::~VertexArray() { glDeleteVertexArrays(1, &m_ID); }

void VertexArray::bind() const { glBindVertexArray(m_ID); }

void VertexArray::unbind() const { glBindVertexArray(0); }
