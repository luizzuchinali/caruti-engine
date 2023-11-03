#version 330 core
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inTexCoords;
layout (location = 2) in vec3 inNormal;

uniform mat4 model;
uniform mat4 camera;

out vec2 TexCoords;

void main() {
    gl_Position = camera * model * vec4(inPos, 1.0);
    TexCoords = inTexCoords;
}