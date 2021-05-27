#version 460 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_color;
layout(location = 2) in vec2 a_tex;
layout(location = 3) in vec3 a_normal;

uniform mat4 u_model;
uniform mat4 u_ti_model;
uniform mat4 u_view;
uniform mat4 u_projection;

out vec3 v_color;
out vec3 v_position;
out vec3 v_normal;

void main() {
    v_position = vec3(u_model * vec4(a_position, 1.0));
    v_color    = a_color;
    v_normal   = mat3(u_ti_model) * a_normal;

    gl_Position = u_projection * u_view * vec4(v_position, 1.0);
}
