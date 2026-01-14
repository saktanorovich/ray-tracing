#version 410 core

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;

uniform mat4 u_mvp;
uniform mat4 u_model;
uniform mat3 u_normal_matrix;

out vec3 v_world_pos;
out vec3 v_normal;

void main() {
    v_world_pos = vec3(u_model * vec4(in_position, 1.0));
    v_normal    = u_normal_matrix * in_normal;
    gl_Position = u_mvp * vec4(in_position, 1.0);
}
