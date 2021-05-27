#include <iostream>
#include <map>
#include <string>

#include "debug.hpp"

std::map<GLenum, std::string> enum_to_string = {
    {GL_DEBUG_SOURCE_API, "OpenGL API"},
    {GL_DEBUG_SOURCE_WINDOW_SYSTEM, "window-system API"},
    {GL_DEBUG_SOURCE_SHADER_COMPILER, "shader compiler"},
    {GL_DEBUG_SOURCE_THIRD_PARTY, "third party"},
    {GL_DEBUG_SOURCE_APPLICATION, "application"},
    {GL_DEBUG_SOURCE_OTHER, "other"},

    {GL_DEBUG_TYPE_ERROR, "error"},
    {GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR, "deprecated"},
    {GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR, "undefined"},
    {GL_DEBUG_TYPE_PORTABILITY, "portability"},
    {GL_DEBUG_TYPE_PERFORMANCE, "performance"},
    {GL_DEBUG_TYPE_MARKER, "annotation"},
    {GL_DEBUG_TYPE_PUSH_GROUP, "push"},
    {GL_DEBUG_TYPE_POP_GROUP, "pop"},
    {GL_DEBUG_TYPE_OTHER, "other"},

    {GL_DEBUG_SEVERITY_HIGH, "high"},
    {GL_DEBUG_SEVERITY_MEDIUM, "medium"},
    {GL_DEBUG_SEVERITY_LOW, "low"},
    {GL_DEBUG_SEVERITY_NOTIFICATION, "notification"},
};

void APIENTRY debug_callback(
    GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei, const GLchar *message, const void *) {
    std::cerr << "[debug_callback] id=" << id << " source=" << enum_to_string[source]
              << " type=" << enum_to_string[type] << " severity=" << enum_to_string[severity] << " message=" << message
              << "\n";
}
