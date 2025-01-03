#pragma once

#include "../util/evaluate.glsl"
#include "../util/light.glsl"

vec3 brdf(vec3 light_dir, vec3 view_dir, Material material)
{
    vec3 normal = normalize(material.normal);
    vec3 albedo = pow(material.diffuse, vec3(2.2f));

    vec3 half_vec = normalize(light_dir + view_dir);
    float cos_N_L = max(dot(normal, light_dir), 0.0);
    float cos_N_V = max(dot(normal, view_dir), 0.0);
    float cos_N_H = max(dot(normal, half_vec), 0.0);
    float cos_V_H = max(dot(view_dir, half_vec), 0.0);
    float cos_L_H = max(dot(light_dir, half_vec), 0.0);

    float distribution = distribution_GGX(material.roughness, cos_N_H);
    float visibility = visibility_Smith(material.roughness, cos_N_V, cos_N_L);

    vec3 F0 = mix(vec3(0.04), albedo, material.metallic);
    vec3 fresnel = fresnel_Schlick(F0, cos_V_H);

    vec3 numerator    = distribution * visibility * fresnel;
    float denominator = (4.0 * cos_N_L * cos_N_V) + 0.0001;
    vec3 specular = numerator / denominator;

    vec3 kD = vec3(1.0) - fresnel;
    kD *= 1.0 - material.metallic;

    return (kD * diffuse_Lambert(albedo) + specular) * cos_N_L;
}

vec3 evaluate_ibl(vec3 world_pos, vec3 eye_pos, Material material)
{
    vec3 albedo = pow(material.diffuse, vec3(2.2));
    float roughness = material.roughness;
    vec3 normal = normalize(material.normal);

    vec3 view_dir = normalize(eyePos - world_pos);
    float cos_N_V = max(dot(normal, view_dir), 0.0);
    vec3 reflect_dir = reflect(-view_dir, normal);

    vec3 F0 = mix(vec3(0.04), albedo, material.metallic);
    vec3 F = fresnel_Schlick_roughness(cos_N_V, F0, roughness);

    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= (1.0 - material.metallic);

    vec3 irradiance = texture(irrandiance_map, material.normal).rgb;
    vec3 diffuse = irradiance * albedo;

    const float MAX_REFLECTION_LOD = 4.0f;
    vec3 prefiltered_color = textureLod(prefiltered_map, reflect_dir, roughness * MAX_REFLECTION_LOD).rgb;
    vec2 brdf = texture(precomputed_brdf, vec2(cos_N_V, roughness)).rg;
    vec3 specular = prefiltered_color * (F * brdf.x + brdf.y);
    return (kD * diffuse + specular) * material.ao;
}

vec3 evaluate_brdf(vec3 world_pos, vec3 eye_pos, Material material)
{
    vec3 result = vec3(0.0);

    // Evaluate ambient.
    // sRGB -> Linear Space.
    vec3 albedo = pow(material.diffuse, vec3(2.2));
    vec3 ambient = vec3(0.0);
    if (use_ibl_data) {
        ambient = evaluate_ibl(world_pos, eye_pos, material);
    } else {
        ambient = ambient_color * albedo * material.ao;
    }

    // Evaluate reflectance equation.
    vec3 Lo = vec3(0.0);
    for (int i = 0; i < point_light_num; i++) {
        vec3 light_dir;
        vec3 view_dir = normalize(eyePos - world_pos);

        vec3 light_color = evaluate_point_light(point_light[i], world_pos, light_dir);
        Lo += brdf(light_dir, view_dir, material) * light_color;
    }

    for (int i = 0; i < directional_light_num; i++) {
        vec3 light_dir;
        vec3 view_dir = normalize(eyePos - world_pos);

        vec3 light_color = evaluate_directional_light(directional_light[i], view_dir, light_dir);
        float bias = max(0.005 * (1.0 - dot(material.normal, light_dir)), 0.005);

        float visibility = render_shadow ? evaluate_directional_shadow(i, world_pos, bias) : 1.0;
        Lo += brdf(light_dir, view_dir, material) * light_color * visibility;
    }

    for (int i = 0; i < spot_light_num; i++) {
        vec3 light_dir;
        vec3 view_dir = normalize(eyePos - world_pos);

        vec3 light_color = evaluate_spot_light(spot_light[i], world_pos, light_dir);
        float bias = max(0.005 * (1.0 - dot(material.normal, light_dir)), 0.005);

        float visibility = render_shadow ? evaluate_spot_shadow(i, world_pos, bias) : 1.0;
        Lo += brdf(light_dir, view_dir, material) * light_color * visibility;
    }

    result = ambient + Lo + material.emissive;

    return result;
}
