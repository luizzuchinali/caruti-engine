#version 330 core

in vec3 FragPos;
in vec3 Normal;

out vec4 fragOutColor;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightSourcePos;
uniform vec3 cameraPos;

void main() {
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightSourcePos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);

    vec3 diffuse = diff * lightColor;

    float specularStrength = 0.8;
    vec3 viewDir = normalize(cameraPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

    vec3 specular = specularStrength * spec * lightColor;

    fragOutColor = vec4((ambient + diffuse + specular) * objectColor, 1.0);
}