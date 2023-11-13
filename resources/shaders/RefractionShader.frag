#version 330 core

in VS_OUT {
    vec3 Position;
    vec3 Normal;
} fs_in;

uniform vec3 cameraPos;
uniform samplerCube skybox;

out vec4 FragColor;

void main() {
    float ratio = 1.00 / 1.33;
    vec3 i = normalize(fs_in.Position - cameraPos);
    vec3 r = refract(i, normalize(fs_in.Normal), ratio);
    FragColor = vec4(texture(skybox, r).rgb, 1.0);
}