
#version 330 core
in vec3 Frag;
in vec3 Norm;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objectColor;

void main() {
    vec3 norm = normalize(Norm);
    vec3 lightDir = normalize(lightPos - Frag);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 result = (diffuse + 0.1) * objectColor;
    FragColor = vec4(result, 1.0);
}
