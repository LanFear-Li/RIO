#pragma once

#include "../common/parameter.glsl"
#include "../util/math.glsl"

vec3 evaluate_point_light(Point_Light light, vec3 world_pos, out vec3 light_dir)
{
    vec3 light_pos = light.position;
    vec3 light_color = light.color * (light.intensity / 4.0 * PI);

    float light_dis = distance(light_pos, world_pos);
    float attenuation = 1.0 / (1.0 + 0.09 * light_dis + 0.032 * light_dis * light_dis);

    light_color = light_color * attenuation;
    light_dir = normalize(light_pos - world_pos);

    return light_color;
}

vec3 evaluate_directional_light(Directional_Light light, vec3 world_pos, out vec3 light_dir)
{
    vec3 light_color = light.color * light.intensity;
    light_dir = normalize(-light.direction);

    return light_color;
}

vec3 evaluate_spot_light(Spot_Light light, vec3 world_pos, out vec3 light_dir)
{
    vec3 light_pos = light.position;
    light_dir = normalize(light_pos - world_pos);
    vec3 light_dir_ori = normalize(-light.direction);
    float cos_theta = dot(light_dir, light_dir_ori);

    // Check if the light is in the cone of the spot light.
    vec3 light_color = vec3(0.0);
    if (cos_theta > light.outer_cutoff) {
        light_color = light.color * (light.intensity / 4.0 * PI);

        // Light attenuation by distance.
        float light_dis = distance(light_pos, world_pos);
        float attenuation = 1.0 / (1.0 + 0.09 * light_dis + 0.032 * light_dis * light_dis);

        // Light attenuation by angle.
        float epsilon = light.cutoff - light.outer_cutoff;
        float soft_ratio = clamp((cos_theta - light.outer_cutoff) / epsilon, 0.0, 1.0);

        light_color = light_color * attenuation * soft_ratio;
    }

    return light_color;
}
