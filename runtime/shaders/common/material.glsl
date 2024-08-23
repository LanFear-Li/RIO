#pragma once

#include "parameter.glsl"

Material init_material()
{
    Material material;

    material.ambient        = vec3(0.0);
    material.diffuse        = vec3(1.0);
    material.specular       = vec3(0.0);
    material.transparency   = vec3(0.0);
    material.shiness        = 0.0;
    material.ior            = 0.0;

    material.emissive       = vec3(0.0);
    material.roughness      = 1.0;
    material.metallic       = 0.0;

    material.ao             = 1.0;

    material.normal         = vec3(0.0, 1.0, 0.0);

    return material;
}

void collect_material(vec2 texCoord, out Material material)
{
    material.ambient        = _mat_ambient;
    material.diffuse        = _mat_diffuse;
    material.specular       = _mat_specular;
    material.transparency   = _mat_transparency;

    material.emissive       = _mat_emissive;
    material.roughness      = _mat_roughness;
    material.metallic       = _mat_metallic;

    if (use_normal_map == true) {
        material.normal = texture(_texture_normal, texCoord).xyz;
    }

    if (use_ambient_map == true) {
        material.ambient = texture(_texture_ambient, texCoord).xyz;
    }

    if (use_diffuse_map == true) {
        material.diffuse = texture(_texture_diffuse, texCoord).xyz;
    }

    if (use_emissive_map == true) {
        material.emissive = texture(_texture_emissive, texCoord).xyz;
    }

    if (use_metallic_map == true) {
        material.metallic = texture(_texture_metallic, texCoord).x;
    }

    if (use_roughness_map == true) {
        material.roughness = texture(_texture_roughness, texCoord).x;
    }

    if (use_metal_roughness_map == true) {
        material.roughness = texture(_texture_metal_roughness, texCoord).y;
        material.metallic = texture(_texture_metal_roughness, texCoord).z;
    }

    if (use_ao_map == true) {
        material.ao = texture(_texture_ao, texCoord).x;
    }
}
