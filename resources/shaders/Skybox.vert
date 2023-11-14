#version 420 core
layout (location = 0) in vec3 aPos;

layout (std140, binding = 0) uniform Matrices {
    mat4 view;
    mat4 projection;
};

out vec3 TexCoords;

void main()
{
    TexCoords = aPos;
    vec4 pos = projection * mat4(mat3(view)) * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}