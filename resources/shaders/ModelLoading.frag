#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_diffuse2;
    sampler2D texture_diffuse3;
    sampler2D texture_specular1;
    sampler2D texture_specular2;
};

uniform Material material;

void main()
{
    vec4 texColor = texture(material.texture_diffuse1, TexCoords);

    if (texColor.a < 0.5) {
        discard;
    }

    FragColor = texColor;
}