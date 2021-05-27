#pragma once

#include <glad/glad.h>

void APIENTRY debug_callback(
    GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei, const GLchar *message, const void *data);
