#version 330 core

in vec3 FragPos;
in vec3 Normal;

out vec4 fragOutColor;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightSourcePos;

void main() {
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightSourcePos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);

    vec3 diffuse = diff * lightColor;

    fragOutColor = vec4((ambient + diffuse) * objectColor, 1.0);
}