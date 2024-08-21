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
    float term = pow(1.0 - cos_V_H, 5.0);
    return F0 + (1.0 - F0) * term;
}

vec3 brdf(vec3 light_dir, vec3 view_dir, Material material)
{
    float cos_N_L = dot(material.normal, light_dir);

    if (cos_N_L <= 0.0) {
        return vec3(0.0);
    }

    vec3 half_vec = normalize(light_dir + view_dir);
    float cos_N_V = saturate(dot(material.normal, view_dir));
    float cos_N_H = saturate(dot(material.normal, half_vec));
    float cos_V_H = saturate(dot(view_dir, half_vec));
    float cos_L_H = saturate(dot(light_dir, half_vec));

    float distribution = distribution_GGX(material.roughness, cos_N_H);
    float visibility = visibility_Smith_GGX(material.roughness, cos_N_V, cos_N_L);

    vec3 diffuse_color = mix(material.diffuse, vec3(0.0), material.metallic);
    vec3 specular_color = mix(vec3(0.08 * material.specular), material.diffuse, material.metallic);
    vec3 fresnel = Fresnel_Schlick(specular_color, cos_V_H);

    // vec3 numerator    = distribution * visibility * fresnel;
    // float denominator = max((4.0 * cos_N_L * cos_N_V), 0.001);
    // vec3 BRDF = numerator / denominator;
    // return BRDF * cos_N_L;

    vec3 diffuse = diffuse_Lambert(diffuse_color) * cos_N_L;
    vec3 specular = distribution * visibility * fresnel * cos_N_L;

    return diffuse + specular;
}

vec3 evaluate_brdf(vec3 world_pos, vec3 eye_pos, Material material)
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

        result += brdf(light_dir, view_dir, material) * light_color;
    }

    result += material.emissive;

    return result;
}
