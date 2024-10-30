#pragma once

// ---------------------
// Light Definition.
// ---------------------
struct Point_Light
{
    vec3 color;
    float intensity;

    vec3 position;
    float radius;
};

struct Directional_Light
{
    vec3 color;
    float intensity;

    vec3 direction;
};

struct Spot_Light
{
    vec3 color;
    float intensity;

    vec3 position;
    vec3 direction;
    float cutoff;
    float outer_cutoff;
    float radius;
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
    float ao;

    vec3 normal;
};

// ---------------------
// Shading Model.
// ---------------------

#define PHONG           0
#define BLINN_PHONG     1
#define BRDF            2

// ---------------------
// Shadow Method.
// ---------------------

#define SHADOW_BASIC    0
#define SHADOW_PCF      1
#define SHADOW_PCSS     2

// ---------------------
// Uniform Definition.
// ---------------------
uniform vec3                eyePos;

uniform vec3                _mat_ambient;
uniform vec3                _mat_diffuse;
uniform vec3                _mat_specular;
uniform vec3                _mat_transparency;
uniform float               _mat_shiness;
uniform float               _mat_ior;
uniform vec3                _mat_emissive;
uniform float               _mat_roughness;
uniform float               _mat_metallic;

uniform bool                use_normal_map;
uniform bool                use_ambient_map;
uniform bool                use_diffuse_map;
uniform bool                use_specular_map;
uniform bool                use_emissive_map;
uniform bool                use_metallic_map;
uniform bool                use_roughness_map;
uniform bool                use_metal_roughness_map;
uniform bool                use_ao_map;

uniform int                 shading_method;
uniform vec3                ambient_color;

// ---------------------
// Lights & Shadows.
// ---------------------

const int                   MAX_LIGHT_NUM = 2;
uniform Point_Light         point_light[MAX_LIGHT_NUM];
uniform Directional_Light   directional_light[MAX_LIGHT_NUM];
uniform Spot_Light          spot_light[MAX_LIGHT_NUM];
uniform int                 point_light_num;
uniform int                 directional_light_num;
uniform int                 spot_light_num;

uniform mat4                directional_light_matrix[MAX_LIGHT_NUM];
uniform mat4                spot_light_matrix[MAX_LIGHT_NUM];
uniform sampler2D           directional_shadow_map[MAX_LIGHT_NUM];
uniform sampler2D           spot_shadow_map[MAX_LIGHT_NUM];

uniform bool                render_shadow;
uniform int                 shadow_method;

// ---------------------
// Sampler Definition.
// ---------------------
uniform sampler2D           _texture_normal;
uniform sampler2D           _texture_ambient;
uniform sampler2D           _texture_diffuse;
uniform sampler2D           _texture_specular;
uniform sampler2D           _texture_emissive;
uniform sampler2D           _texture_metallic;
uniform sampler2D           _texture_roughness;
uniform sampler2D           _texture_metal_roughness;
uniform sampler2D           _texture_ao;

// ---------------------
// IBL Data for PBR.
// ---------------------
uniform samplerCube         environment_map;
uniform samplerCube         irrandiance_map;
uniform samplerCube         prefiltered_map;
uniform sampler2D           precomputed_brdf;
uniform bool                use_ibl_data;
