#version 330 core

out vec4 fragOutColor;

uniform vec3 objectColor;
uniform vec3 lightColor;

void main() {
    fragOutColor = vec4(lightColor * objectColor, 1.0);
}