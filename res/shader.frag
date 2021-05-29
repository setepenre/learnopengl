#version 460 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light {
    vec4 position;
    vec3 direction;
    float cut_off;
    float outer_cut_off;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

in vec3 v_position;
in vec2 v_tex;
in vec3 v_normal;

uniform vec3 u_view_position;

uniform Material u_material;
uniform Light u_light;

out vec4 color;

void main() {
    vec3 object_color   = texture(u_material.diffuse, v_tex).rgb;
    vec3 specular_color = texture(u_material.specular, v_tex).rgb;
    vec3 norm           = normalize(v_normal);

    float attenuation    = 1.0;
    vec3 light_direction = vec3(0.0);
    if (u_light.position.w == 0.0) {
        light_direction = normalize(-u_light.position.xyz);
    } else {
        light_direction = normalize(u_light.position.xyz - v_position);
        float d         = length(light_direction);
        attenuation     = 1.0 / (u_light.constant + u_light.linear * d + u_light.quadratic * d * d);
    }

    vec3 ambient  = u_light.ambient * object_color;
    vec3 diffuse  = vec3(0.0);
    vec3 specular = vec3(0.0);

    float theta     = dot(light_direction, normalize(-u_light.direction));
    float intensity = clamp((theta - u_light.outer_cut_off) / (u_light.cut_off - u_light.outer_cut_off), 0.0, 1.0);
    diffuse         = u_light.diffuse * (max(dot(norm, light_direction), 0.0) * object_color);
    specular        = u_light.specular *
               pow(max(dot(normalize(u_view_position - v_position), reflect(-light_direction, norm)), 0.0),
                   u_material.shininess) *
               specular_color;

    color = vec4(attenuation * (ambient + intensity * (diffuse + specular)), 1.0);
}
