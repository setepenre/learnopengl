#version 460 core

uniform vec3 u_light_color;

out vec4 color;

void main() { color = vec4(u_light_color, 1.0); }
