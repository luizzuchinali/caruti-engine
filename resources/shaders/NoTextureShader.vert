#version 330 core
layout (location = 0) in vec3 inPos;
layout (location = 2) in vec3 inNormal;

uniform mat4 model;
uniform mat4 camera;

out vec3 FragPos;
out vec3 Normal;

void main() {
    gl_Position = camera * model * vec4(inPos, 1.0);
    FragPos = vec3(model * vec4(inPos, 1.0));
    Normal = inNormal;
}