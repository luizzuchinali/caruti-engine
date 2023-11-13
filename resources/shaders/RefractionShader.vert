#version 330 core
layout (location = 0) in vec3 inPos;
layout (location = 2) in vec3 inNormal;

out VS_OUT {
    vec3 Position;
    vec3 Normal;
} vs_out;

uniform mat4 model;
uniform mat4 camera;

void main() {
    vs_out.Normal = mat3(transpose(inverse(model))) * inNormal;
    vs_out.Position = vec3(model * vec4(inPos, 1));
    gl_Position = camera * model * vec4(inPos, 1);
}