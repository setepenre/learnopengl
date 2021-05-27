#version 460 core

in vec3 v_color;
in vec2 v_tex_coord;

uniform float u_alpha;
uniform sampler2D u_texture0;
uniform sampler2D u_texture1;

out vec4 color;

void main() { color = mix(texture(u_texture0, v_tex_coord), texture(u_texture1, v_tex_coord), u_alpha); }
