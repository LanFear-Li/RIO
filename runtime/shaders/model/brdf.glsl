#pragma once

vec3 diffuse_Lambert(vec3 diffuse_color)
{
    return diffuse_color * INV_PI;
}

// Normal distribution term with GGX.
float distribution_GGX(float roughness, float cos_N_H)
{
    float aPow = pow4(roughness);
    float NdotHPow = cos_N_H * cos_N_H;

    float numerator = aPow;
    float denominator = NdotHPow * (aPow - 1.0) + 1.0;
    denominator = PI * denominator * denominator;

    return numerator / denominator;
}

// Visibility/Geometry term with GGX.
float visibility_Smith_GGX(float roughness, float cos_N_V, float cos_N_L)
{
    float alpha = pow2(roughness);
    float lambda_GGXV = cos_N_L * (cos_N_V * (1.0 - alpha) + alpha);
    float lambda_GGXL = cos_N_V * (cos_N_L * (1.0 - alpha) + alpha);
    return 0.5 / (lambda_GGXV + lambda_GGXL);
}

// Fresnel term with Schlick.
vec3 Fresnel_Schlick(vec3 F0, float cos_V_H)
{
    float term = pow(clamp(1.0 - cos_V_H, 0.0, 1.0), 5.0);
    return F0 + (1.0 - F0) * term;
}

vec3 brdf(vec3 light_dir, vec3 view_dir, Material material)
{
    vec3 normal = normalize(material.normal);
    float cos_N_L = dot(normal, light_dir);
    vec3 albedo = material.diffuse;

    vec3 half_vec = normalize(light_dir + view_dir);
    float cos_N_V = max(dot(normal, view_dir), 0.0);
    float cos_N_H = max(dot(normal, half_vec), 0.0);
    float cos_V_H = max(dot(view_dir, half_vec), 0.0);
    float cos_L_H = max(dot(light_dir, half_vec), 0.0);

    float distribution = distribution_GGX(material.roughness, cos_N_H);
    float visibility = visibility_Smith_GGX(material.roughness, cos_N_V, cos_N_L);

    vec3 F0 = mix(vec3(0.04), albedo, material.metallic);
    vec3 fresnel = Fresnel_Schlick(F0, cos_V_H);

    vec3 numerator    = distribution * visibility * fresnel;
    float denominator = (4.0 * cos_N_L * cos_N_V) + 0.001;
    vec3 specular = numerator / denominator;

    vec3 kD = vec3(1.0) - fresnel;
    kD *= 1.0 - material.metallic;

    return (kD * diffuse_Lambert(albedo) + specular) * cos_N_L;
}

vec3 evaluate_brdf(vec3 world_pos, vec3 eye_pos, Material material)
{
    vec3 result = vec3(0.0);

    // Evaluate ambient.
    vec3 albedo = material.diffuse;
    vec3 ambient = vec3(0.03) * albedo * material.ao;

    // Evaluate reflectance equation.
    vec3 Lo = vec3(0.0);
    for (int i = 0; i < point_light_num; i++) {
        vec3 light_pos = point_light[i].position;
        vec3 light_color = point_light[i].color * (point_light[i].intensity / 4.0 * PI);

        float light_dis = distance(light_pos, world_pos);
        float attenuation = 1.0 / (1.0 + 0.09 * light_dis + 0.032 * light_dis * light_dis);
        light_color = light_color * attenuation;

        vec3 light_dir = normalize(light_pos - world_pos);
        vec3 view_dir = normalize(eyePos - world_pos);

        Lo += brdf(light_dir, view_dir, material) * light_color;
    }

    result = ambient + Lo + material.emissive;

    return result;
}
