#version 330 core

in VS_OUT {
    vec3 Position;
    vec3 Normal;
} fs_in;

uniform vec3 cameraPos;
uniform samplerCube skybox;

out vec4 FragColor;

void main() {
    vec3 i = normalize(fs_in.Position - cameraPos);
    vec3 normalizedNormal = normalize(fs_in.Normal);

    //Reflection formula
    vec3 r = i - 2 * dot(normalizedNormal, i) * normalizedNormal;
    FragColor = vec4(texture(skybox, r).rgb, 1.0);
}