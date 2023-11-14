#version 420 core
layout (location = 0) in vec3 inPos;
layout (location = 2) in vec3 inNormal;

layout (std140, binding = 0) uniform Matrices {
    mat4 view;
    mat4 projection;
};

out VS_OUT {
    vec3 Position;
    vec3 Normal;
} vs_out;

uniform mat4 model;

void main() {
    vs_out.Normal = mat3(transpose(inverse(model))) * inNormal;
    vs_out.Position = vec3(model * vec4(inPos, 1));
    gl_Position = projection * view * model * vec4(inPos, 1);
}