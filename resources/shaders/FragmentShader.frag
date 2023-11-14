#version 420 core

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

out vec4 fragOutColor;

uniform sampler2D tex;

void main() {
    fragOutColor = texture(tex, fs_in.TexCoords);
}