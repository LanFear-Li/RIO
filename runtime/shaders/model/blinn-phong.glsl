#pragma once

#include "../common/parameter.glsl"
#include "../common/material.glsl"
#include "../util/math.glsl"

vec3 phong(vec3 light_dir, vec3 view_dir, Material material)
{
    vec3 object_color = material.diffuse;

    // Diffuse contribution.
    vec3 norm = normalize(material.normal);
    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = diff * object_color;

    // Specular contribution.
    float specular_exponent = 128.0;
    vec3 reflect_dir = reflect(-light_dir, norm);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), specular_exponent);
    vec3 specular = spec * vec3(1.0);

    return diffuse + specular;
}

vec3 blinn_phong(vec3 light_dir, vec3 view_dir, Material material)
{
    vec3 object_color = material.diffuse;

    // Diffuse contribution.
    vec3 norm = normalize(material.normal);
    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = diff * object_color;

    // Specular contribution.
    float specular_exponent = 128.0;
    vec3 half_vec = normalize(light_dir + view_dir);
    float spec = pow(max(dot(norm, half_vec), 0.0), specular_exponent);
    vec3 specular = vec3(spec);

    return diffuse + specular;
}

vec3 evaluate_phong(vec3 world_pos, vec3 eye_pos, Material material)
{
    // Evaluate ambient.
    // sRGB -> Linear Space.
    vec3 albedo = pow(material.diffuse, vec3(2.2f));
    vec3 ambient = ambient_color * albedo * material.ao;

    vec3 result = vec3(0.0);
    for (int i = 0; i < point_light_num; i++) {
        vec3 light_pos = point_light[i].position;
        vec3 light_color = point_light[i].color * (point_light[i].intensity / 4.0 * PI);

        float light_dis = distance(light_pos, world_pos);
        float attenuation = 1.0 / (1.0 + 0.09 * light_dis + 0.032 * light_dis * light_dis);
        light_color = light_color * attenuation;

        vec3 light_dir = normalize(light_pos - world_pos);
        vec3 view_dir = normalize(eyePos - world_pos);

        result += phong(light_dir, view_dir, material) * light_color;
    }

    result += ambient + material.emissive;

    return result;
}

vec3 evaluate_blinn_phong(vec3 world_pos, vec3 eye_pos, Material material)
{
    // Evaluate ambient.
    // sRGB -> Linear Space.
    vec3 albedo = pow(material.diffuse, vec3(2.2f));
    vec3 ambient = ambient_color * albedo * material.ao;

    vec3 result = vec3(0.0);
    for (int i = 0; i < point_light_num; i++) {
        vec3 light_pos = point_light[i].position;
        vec3 light_color = point_light[i].color * (point_light[i].intensity / 4.0 * PI);

        float light_dis = distance(light_pos, world_pos);
        float attenuation = 1.0 / (1.0 + 0.09 * light_dis + 0.032 * light_dis * light_dis);
        light_color = light_color * attenuation;

        vec3 light_dir = normalize(light_pos - world_pos);
        vec3 view_dir = normalize(eyePos - world_pos);

        result += blinn_phong(light_dir, view_dir, material) * light_color;
    }

    result += ambient + material.emissive;

    return result;
}
