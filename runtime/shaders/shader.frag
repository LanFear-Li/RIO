#include "util/math.glsl"

out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D texture_diffuse1;

uniform vec3 viewPos;

struct Point_Light
{
    vec3 position;
    vec3 color;
    float radius;
    float intensity;
};

const int MAX_LIGHT_NUM = 16;
uniform Point_Light point_light[MAX_LIGHT_NUM];
uniform int point_light_num;

vec3 phong(vec3 objectColor, vec3 lightColor, vec3 lightPos)
{
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;

    return result;
}

vec3 blinn_phong(vec3 objectColor, vec3 lightColor, vec3 lightPos)
{
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 half_vec = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, half_vec), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;

    return result;
}

void main()
{
    vec3 objectColor = texture(texture_diffuse1, TexCoords).xyz;

    vec3 result = vec3(0.0);
    for (int i = 0; i < point_light_num; i++) {
        vec3 pos = point_light[i].position;
        vec3 color = point_light[i].color;

        result += blinn_phong(objectColor, color, pos);
    }

    FragColor = vec4(result, 1.0);
}
