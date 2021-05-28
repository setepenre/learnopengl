#version 460 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float shininess;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 v_position;
in vec3 v_normal;

uniform vec3 u_object_color;
uniform vec3 u_view_position;

uniform Material u_material;
uniform Light u_light;

out vec4 color;

void main() {
    vec3 norm            = normalize(v_normal);
    vec3 light_direction = normalize(u_light.position - v_position);

    vec3 ambient  = u_light.ambient * u_material.ambient;
    vec3 diffuse  = u_light.diffuse * (max(dot(norm, light_direction), 0.0) * u_material.diffuse);
    vec3 specular = u_light.specular *
                    (0.5 *
                        pow(max(dot(normalize(u_view_position - v_position), reflect(-light_direction, norm)), 0.0),
                            u_material.shininess) *
                        u_material.specular);
    color = vec4((ambient + diffuse + specular) * u_object_color, 1.0);
}
