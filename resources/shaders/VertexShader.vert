#version 330 core
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec2 inTexCoord;

out vec4 vertOutColor;
out vec2 vertOutTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(inPos, 1.0);

    vertOutColor = vec4(inColor, 1);
    vertOutTexCoord = inTexCoord;
}