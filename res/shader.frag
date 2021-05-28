#version 460 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;

    sampler2D emissive;
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
uniform float u_tex_t_offset;

uniform Material u_material;
uniform Light u_light;

out vec4 color;

void main() {
    vec3 object_color    = texture(u_material.diffuse, v_tex).rgb;
    vec3 specular_color  = texture(u_material.specular, v_tex).rgb;
    vec3 norm            = normalize(v_normal);
    vec3 light_direction = normalize(u_light.position - v_position);

    vec3 ambient  = u_light.ambient * object_color;
    vec3 diffuse  = u_light.diffuse * (max(dot(norm, light_direction), 0.0) * object_color);
    vec3 specular = u_light.specular *
                    pow(max(dot(normalize(u_view_position - v_position), reflect(-light_direction, norm)), 0.0),
                        u_material.shininess) *
                    specular_color;

    vec3 emissive = step(vec3(1.0), vec3(1.0) - specular_color) *
                    texture(u_material.emissive, v_tex + u_tex_t_offset * vec2(0.0, 1.0)).rgb;
    color = vec4(ambient + diffuse + specular + emissive, 1.0);
}
