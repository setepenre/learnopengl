#version 460 core

in vec3 v_position;
in vec3 v_normal;

uniform vec3 u_object_color;
uniform vec3 u_light_position;
uniform vec3 u_light_color;
uniform vec3 u_view_position;

out vec4 color;

void main() {
    vec3 norm            = normalize(v_normal);
    vec3 light_direction = normalize(u_light_position - v_position);

    float ambient = 0.1;
    float diffuse = max(dot(norm, light_direction), 0.0);
    float specular =
        0.5 * pow(max(dot(normalize(u_view_position - v_position), reflect(-light_direction, norm)), 0.0), 32);
    color = vec4((ambient + diffuse + specular) * u_light_color * u_object_color, 1.0);
}
