#include "texture.hpp"

#include <glad/glad.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include <stb_image.h>
#pragma GCC diagnostic pop

std::pair<Texture, Error> Texture::from_file(const std::string &path, int format, int texture_slot,
    const TextureParameters &parameters, bool gen_mipmap, bool flip_y) {
    stbi_set_flip_vertically_on_load(flip_y);

    int width {0}, height {0}, n_channels {0};
    unsigned char *pixels = stbi_load(path.c_str(), &width, &height, &n_channels, 0);
    if (! pixels) {
        return {Texture(0, 0), wrap(std::string(stbi_failure_reason()) + " " + path)};
    }

    unsigned int ID = 0;
    glGenTextures(1, &ID);

    glActiveTexture(texture_slot);
    glBindTexture(GL_TEXTURE_2D, ID);

    for (auto [parameter, value] : parameters) {
        glTexParameteri(GL_TEXTURE_2D, parameter, value);
    }

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, pixels);

    if (gen_mipmap) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    stbi_image_free(pixels);

    return {{ID, texture_slot}, {}};
}

void Texture::bind() const {
    glActiveTexture(m_slot);
    glBindTexture(GL_TEXTURE_2D, m_ID);
}
void Texture::unbind() const { glBindTexture(GL_TEXTURE_2D, 0); }
