#version 330 core
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inTexCoord;

out vec2 vertOutTexCoord;

uniform mat4 model;
uniform mat4 camera;

void main() {
    gl_Position = camera * model * vec4(inPos, 1.0);
    vertOutTexCoord = inTexCoord;
}