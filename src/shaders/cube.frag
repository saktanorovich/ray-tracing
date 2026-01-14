#version 410 core

in vec3 v_world_pos;
in vec3 v_normal;

out vec4 out_color;

uniform vec3 u_light_pos;
uniform vec3 u_light_color;
uniform vec3 u_object_color;
uniform vec3 u_view_pos;

const float ambient_strength  =  0.08;
const float specular_strength =  0.35;
const float shininess         = 64.00;

// Blinn-Phong evaluated in linear space, encoded to sRGB at the end.
void main() {
    vec3 n = normalize(v_normal);
    vec3 l = normalize(u_light_pos - v_world_pos);
    vec3 v = normalize(u_view_pos  - v_world_pos);
    vec3 h = normalize(l + v);

    vec3 ambient  = ambient_strength * u_light_color;
    vec3 diffuse  = max(dot(n, l), 0.0) * u_light_color;
    vec3 specular = pow(max(dot(n, h), 0.0), shininess) * specular_strength * u_light_color;

    vec3 linear = (ambient + diffuse) * u_object_color + specular;
    out_color = vec4(pow(linear, vec3(1.0 / 2.2)), 1.0);
}
