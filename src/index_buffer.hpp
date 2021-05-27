#pragma once

#include <vector>

class IndexBuffer {
  public:
    IndexBuffer(std::vector<unsigned int> &&data);
    ~IndexBuffer();

    void bind() const;
    void unbind() const;

    unsigned int count() const { return m_count; }

  private:
    unsigned int m_ID;
    unsigned int m_count;
};
