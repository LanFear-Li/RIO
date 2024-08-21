#pragma once

#include "../common/parameter.glsl"
#include "../common/material.glsl"
#include "../util/math.glsl"

vec3 phong(vec3 light_dir, vec3 view_dir, Material material)
{
    vec3 object_color = material.diffuse;

    // Ambient contribution.
    vec3 ambient = vec3(0.1, 0.1, 0.1);

    // Diffuse contribution.
    vec3 norm = normalize(material.normal);
    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = diff * vec3(1.0);

    // Specular contribution.
    float specular_strength = 0.5;
    vec3 reflect_dir = reflect(-light_dir, norm);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
    vec3 specular = specular_strength * spec * vec3(1.0);

    vec3 result = (ambient + diffuse + specular) * object_color;

    return result;
}

vec3 blinn_phong(vec3 light_dir, vec3 view_dir, Material material)
{
    vec3 object_color = material.diffuse;

    // Ambient contribution.
    vec3 ambient = vec3(0.1, 0.1, 0.1);

    // Diffuse contribution.
    vec3 norm = normalize(material.normal);
    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = diff * vec3(1.0);

    // Specular contribution.
    float specular_strength = 0.5;
    vec3 half_vec = normalize(light_dir + view_dir);
    float spec = pow(max(dot(norm, half_vec), 0.0), 32);
    vec3 specular = specular_strength * spec * vec3(1.0);

    vec3 result = (ambient + diffuse + specular) * object_color;

    return result;
}

vec3 evaluate_phong(vec3 world_pos, vec3 eye_pos, Material material)
{
    vec3 result = vec3(0.0);
    for (int i = 0; i < point_light_num; i++) {
        vec3 light_pos = point_light[i].position;
        vec3 light_color = point_light[i].color * (point_light[i].intensity / 4.0 * PI);

        vec3 light_dir = normalize(light_pos - world_pos);
        vec3 view_dir = normalize(eyePos - world_pos);

        result += phong(light_dir, view_dir, material) * light_color;
    }

    return result;
}

vec3 evaluate_blinn_phong(vec3 world_pos, vec3 eye_pos, Material material)
{
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

    return result;
}
