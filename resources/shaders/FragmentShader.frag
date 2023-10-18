#version 330 core

in vec4 vertOutColor;
in vec2 vertOutTexCoord;

out vec4 fragOutColor;

uniform sampler2D tex;

void main() {
    fragOutColor = texture(tex, vertOutTexCoord);
}