#version 460 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 v_position;
in vec2 v_tex;
in vec3 v_normal;

uniform vec3 u_view_position;

uniform Material u_material;
uniform Light u_light;

out vec4 color;

void main() {
    vec3 norm            = normalize(v_normal);
    vec3 light_direction = normalize(u_light.position - v_position);
    vec3 object_color    = vec3(texture(u_material.diffuse, v_tex));

    vec3 ambient  = u_light.ambient * object_color;
    vec3 diffuse  = u_light.diffuse * (max(dot(norm, light_direction), 0.0) * object_color);
    vec3 specular = u_light.specular *
                    (0.5 * pow(max(dot(normalize(u_view_position - v_position), reflect(-light_direction, norm)), 0.0),
                               u_material.shininess)) *
                    vec3(texture(u_material.specular, v_tex));
    color = vec4((ambient + diffuse + specular), 1.0);
}
