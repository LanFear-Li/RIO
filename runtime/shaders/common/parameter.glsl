#pragma once

// ---------------------
// Light Definition.
// ---------------------
struct Point_Light
{
    vec3 position;
    vec3 color;
    float radius;
    float intensity;
};

// ---------------------
// Material Definition.
// ---------------------
struct Material
{
    // Basic property.
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 transparency;
    float shiness;
    float ior;

    // PBR property.
    vec3 emissive;
    float roughness;
    float metallic;

    vec3 normal;
};

// ---------------------
// Uniform Definition.
// ---------------------
uniform vec3        eyePos;

uniform vec3        _mat_ambient;
uniform vec3        _mat_diffuse;
uniform vec3        _mat_specular;
uniform vec3        _mat_transparency;
uniform float       _mat_shiness;
uniform float       _mat_ior;
uniform vec3        _mat_emissive;
uniform float       _mat_roughness;
uniform float       _mat_metallic;

uniform bool        use_diffuse_map;
uniform bool        use_specular_map;
uniform bool        use_emissive_map;
uniform bool        use_metallic_map;
uniform bool        use_roughness_map;
uniform bool        use_normal_map;

const int           MAX_LIGHT_NUM = 16;
uniform Point_Light point_light[MAX_LIGHT_NUM];
uniform int         point_light_num;

// ---------------------
// Sampler Definition.
// ---------------------
uniform sampler2D   _texture_diffuse;
uniform sampler2D   _texture_specular;
uniform sampler2D   _texture_emissive;
uniform sampler2D   _texture_metallic;
uniform sampler2D   _texture_roughness;
uniform sampler2D   _texture_normal;