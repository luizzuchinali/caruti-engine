#version 420 core

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_diffuse2;
    sampler2D texture_diffuse3;
    sampler2D texture_specular1;
    sampler2D texture_specular2;

    //    sampler2D diffuse;
    //    sampler2D specular;
    float shininess;
};

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct DirectionalLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define MAX_POINT_LIGHTS 128
uniform int pointLightCount = 0;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform DirectionalLight dirLight;
uniform SpotLight spotLight;

uniform Material material;
uniform sampler2D shadowMap;
uniform vec3 cameraPos;

out vec4 FragOutColor;

vec4 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir, float shadow)
{
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

    vec4 texelAlpha = texture(material.texture_diffuse1, fs_in.TexCoords);
    vec3 texelNoAlpha = texelAlpha.rgb;

    vec3 ambient = light.ambient * texelNoAlpha;
    vec3 diffuse = light.diffuse * diff * texelNoAlpha;
    vec3 specular = light.specular * spec * texelNoAlpha;
    return vec4(ambient + (1.0 - shadow) * (diffuse + specular), texelAlpha.a);
}

vec4 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, float shadow)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    //    float attenuation = 1.0 / distance;

    vec4 texelAlpha = texture(material.texture_diffuse1, fs_in.TexCoords);
    vec3 texelNoAlpha = texelAlpha.rgb;

    vec3 ambient = light.ambient * texelNoAlpha;
    vec3 diffuse = light.diffuse * diff * texelNoAlpha;
    vec3 specular = light.specular * spec * texelNoAlpha;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return vec4(ambient + (1.0 - shadow) * (diffuse + specular), texelAlpha.a);
}

vec4 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, float shadow)
{
    vec3 lightDir = normalize(light.position - fragPos);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    //    float attenuation = 1.0 / distance;

    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    vec4 texelAlpha = texture(material.texture_diffuse1, fs_in.TexCoords);
    vec3 texelNoAlpha = texelAlpha.rgb;

    vec3 ambient = light.ambient * texelNoAlpha;
    vec3 diffuse = light.diffuse * diff * texelNoAlpha;
    vec3 specular = light.specular * spec * texelNoAlpha;

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return vec4(ambient + (1.0 - shadow) * (diffuse + specular), texelAlpha.a);
}

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float bias = 0.005;
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

    return shadow;
}

void main() {
    vec4 texColor = texture(material.texture_diffuse1, fs_in.TexCoords);
    if (texColor.a < 0.1) {
        discard;
    }

    vec4 fragOutput = vec4(0);
    vec3 norm = normalize(fs_in.Normal);
    vec3 viewDir = normalize(cameraPos - fs_in.FragPos);

    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);

    fragOutput += CalcDirLight(dirLight, fs_in.Normal, viewDir, shadow);
    if (pointLightCount > 0) {
        for (int i = 0; i < pointLightCount; i++) {
            fragOutput += CalcPointLight(pointLights[i], fs_in.Normal, fs_in.FragPos, viewDir, shadow);
        }
    }

    if (length(spotLight.direction) > 0) {
        fragOutput += CalcSpotLight(spotLight, fs_in.Normal, fs_in.FragPos, viewDir, shadow);
    }

    float gamma = 2.2;
    FragOutColor.rgb = pow(fragOutput.rgb, vec3(1.0 / gamma));
}