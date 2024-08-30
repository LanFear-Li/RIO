#pragma once

#include "../util/math.glsl"

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

float visibility_Schlick_GGX(float roughness, float cos_N_V)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float numerator   = cos_N_V;
    float denominator = cos_N_V * (1.0 - k) + k;

    return numerator / denominator;
}

// Visibility/Geometry term with Smith GGX.
float visibility_Smith(float roughness, float cos_N_V, float cos_N_L)
{
    float ggx1 = visibility_Schlick_GGX(roughness, cos_N_V);
    float ggx2 = visibility_Schlick_GGX(roughness, cos_N_L);
    return ggx1 * ggx2;
}

// Fresnel term with Schlick.
vec3 Fresnel_Schlick(vec3 F0, float cos_V_H)
{
    float term = pow(clamp(1.0 - cos_V_H, 0.0, 1.0), 5.0);
    return F0 + (1.0 - F0) * term;
}

vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0f - roughness), F0) - F0) * pow(clamp(1.0f - cosTheta, 0.0f, 1.0f), 5.0f);
}

// Efficient VanDerCorpus calculation.
float RadicalInverse_VdC(uint bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10;// / 0x100000000
}

vec2 Hammersley(uint i, uint N)
{
    return vec2(float(i) / float(N), RadicalInverse_VdC(i));
}

vec3 importance_sample_GGX(float roughness, vec2 Xi, vec3 N)
{
    float a = roughness * roughness;

    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a * a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    // From spherical coordinates to cartesian coordinates - halfway vector.
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;

    // From tangent-space H vector to world-space sample vector.
    vec3 up = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);

    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}
