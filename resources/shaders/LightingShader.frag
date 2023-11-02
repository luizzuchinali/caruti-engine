#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light {
    vec4 vector;

    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

uniform Material material;
uniform Light light;
uniform vec3 cameraPos;

out vec4 FragOutColor;

void main() {

    // ambient
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;

    // specular
    vec3 viewDir = normalize(cameraPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;

    // spotlight (soft edges)
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = (light.cutOff - light.outerCutOff);
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    diffuse  *= intensity;
    specular *= intensity;

    // attenuation
    float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    ambient  *= attenuation;
    diffuse   *= attenuation;
    specular *= attenuation;

    FragOutColor = vec4(ambient + diffuse + specular, 1.0);

    //    vec3 lightDir;
    //    if (light.vector.w == 0) {
    //        lightDir = normalize(-light.vector.xyz);
    //
    //        vec3 ambient = light.ambient * vec3(texture2D(material.diffuse, TexCoords));
    //
    //        vec3 norm = normalize(Normal);
    //        float diff = max(dot(norm, lightDir), 0.0);
    //        vec3 diffuse = light.diffuse * diff * vec3(texture2D(material.diffuse, TexCoords));
    //
    //        vec3 viewDir = normalize(cameraPos - FragPos);
    //        vec3 reflectDir = reflect(-lightDir, norm);
    //        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    //        vec3 specular = light.specular * spec * vec3(texture2D(material.specular, TexCoords));
    //
    //        FragOutColor = vec4(ambient + diffuse + specular, 1.0);
    //    }
    //    else if (light.vector.w == 1.0) {
    //        float distance = length(light.vector.xyz - FragPos);
    //        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    //
    //        lightDir = normalize(light.vector.xyz - FragPos);
    //
    //        vec3 ambient = light.ambient * vec3(texture2D(material.diffuse, TexCoords));
    //
    //        vec3 norm = normalize(Normal);
    //        float diff = max(dot(norm, lightDir), 0.0);
    //        vec3 diffuse = light.diffuse * diff * vec3(texture2D(material.diffuse, TexCoords));
    //
    //        vec3 viewDir = normalize(cameraPos - FragPos);
    //        vec3 reflectDir = reflect(-lightDir, norm);
    //        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    //        vec3 specular = light.specular * spec * vec3(texture2D(material.specular, TexCoords));
    //
    //        ambient *= attenuation;
    //        diffuse *= attenuation;
    //        specular *= attenuation;
    //
    //        FragOutColor = vec4(ambient + diffuse + specular, 1.0);
    //    }
}