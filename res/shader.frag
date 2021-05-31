#version 460 core

struct Material {
    vec4 color;
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light {
    vec4 position;

    bool is_directional;
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
uniform int u_nlights;
uniform Light u_lights[8];

out vec4 color;

vec3 phong(Light light, vec3 fragment_position, vec3 normal, vec3 object_color, float shininess, vec3 specular_color,
    vec3 view_direction) {
    float attenuation    = 1.0;
    vec3 light_direction = vec3(0.0);
    if (light.position.w == 0.0) {
        light_direction = normalize(-light.position.xyz);
    } else {
        light_direction = normalize(light.position.xyz - fragment_position);
        float d         = length(light_direction);
        attenuation     = 1.0 / (light.constant + light.linear * d + light.quadratic * d * d);
    }

    vec3 ambient = light.ambient * object_color;

    float intensity;
    if (light.is_directional) {
        float theta = dot(light_direction, normalize(-light.direction));
        intensity   = clamp((theta - light.outer_cut_off) / (light.cut_off - light.outer_cut_off), 0.0, 1.0);
    } else {
        intensity = 1.0f;
    }

    vec3 diffuse = light.diffuse * (max(dot(normal, light_direction), 0.0) * object_color);
    vec3 specular =
        light.specular *
        pow(max(dot(normalize(view_direction - v_position), reflect(-light_direction, normal)), 0.0), shininess) *
        specular_color;

    return attenuation * (ambient + intensity * (diffuse + specular));
}

void main() {
    bool is_solid       = u_material.color.a != 0.0;
    vec3 object_color   = is_solid ? u_material.color.rgb : texture(u_material.diffuse, v_tex).rgb;
    vec3 specular_color = is_solid ? u_material.color.rgb : texture(u_material.specular, v_tex).rgb;
    vec3 normal         = normalize(v_normal);

    vec3 acc = vec3(0.0);
    for (int i = 0; i < u_nlights; i++) {
        if (! (i < 8)) {
            break;
        }
        acc +=
            phong(u_lights[i], v_position, normal, object_color, u_material.shininess, specular_color, u_view_position);
    }
    color = vec4(acc, 1.0);
}
