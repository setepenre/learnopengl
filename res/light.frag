#version 460 core

uniform vec3 u_object_color;
uniform vec3 u_light_color;

out vec4 color;

void main() { color = vec4(u_light_color * u_object_color, 1.0); }
