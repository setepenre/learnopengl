#include "vertex_buffer.hpp"

VertexBuffer::VertexBuffer(std::vector<Vertex> &&vertices) {
    glGenBuffers(1, &m_ID);
    glBindBuffer(GL_ARRAY_BUFFER, m_ID);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer() { glDeleteBuffers(1, &m_ID); }

void VertexBuffer::bind() const { glBindBuffer(GL_ARRAY_BUFFER, m_ID); }

void VertexBuffer::unbind() const { glBindBuffer(GL_ARRAY_BUFFER, 0); };
