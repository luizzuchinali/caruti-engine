#version 330 core
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inColor;

out vec4 vertOutColor;

void main() {
    gl_Position = vec4(inPos.x, inPos.y, inPos.z, 1.0);

    vertOutColor = vec4(inColor, 1);
}