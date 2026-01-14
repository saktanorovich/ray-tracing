
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

out vec3 Frag;
out vec3 Norm;

uniform mat4 MVP;
uniform mat4 model;

void main() {
    Frag = vec3(model * vec4(aPos, 1.0));
    Norm = mat3(transpose(inverse(model))) * aNormal;
    gl_Position = MVP * vec4(aPos, 1.0);
}
