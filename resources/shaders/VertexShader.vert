#version 420 core
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inTexCoords;
layout (location = 2) in vec3 inNormal;

layout (std140, binding = 0) uniform Matrices {
    mat4 view;
    mat4 projection;
};

uniform mat4 model;
uniform mat4 lightSpaceMatrix;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} vs_out;

void main() {
    gl_Position = projection * view * model * vec4(inPos, 1.0);
    vs_out.FragPos = vec3(model * vec4(inPos, 1.0));
    vs_out.Normal = normalize(mat3(transpose(inverse(model))) * inNormal);
    vs_out.TexCoords = inTexCoords;
    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
}