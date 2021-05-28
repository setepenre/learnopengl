#pragma once

#include <vector>

#include <glad/glad.h>

#include "error.hpp"

using TextureParameter  = std::pair<int, int>;
using TextureParameters = std::vector<TextureParameter>;

class Texture {
  public:
    static std::pair<Texture, Error> from_file(const std::string &path, int texture_slot,
        const TextureParameters &parameters, bool gen_mipmap = true, bool flip_y = false);

    void bind() const;
    void unbind() const;

    unsigned int ID() const { return m_ID; };
    int slot() const { return m_slot - GL_TEXTURE0; };

  private:
    Texture(unsigned int ID, int slot) : m_ID(ID), m_slot(slot) {};
    unsigned int m_ID;
    int m_slot;
};
