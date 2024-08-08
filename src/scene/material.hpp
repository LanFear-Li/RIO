#pragma once

#include "scene/texture.hpp"

#include <glm/glm.hpp>

#include <iostream>
#include <memory>

struct Material
{
    std::string mat_name{};

    // --------------------------------------
    // Common property defined in MTL file.
    // See https://en.wikipedia.org/wiki/Wavefront_.obj_file#Material_template_library.
    // --------------------------------------

    // Ka: ambient reflectivity.
    glm::vec3 ambient{};
    // Kd: diffuse reflectivity.
    glm::vec3 diffuse{};
    // Ks: specular reflectivity.
    glm::vec3 specular{};
    // Tr: transparency.
    // Some implementations use 'd' as 'dissolve' and Tr = 1 - d.
    // Tf: transmission filter color (additionally with Tr).
    glm::vec3 transparency{};
    // Ns: shiniess or specular highlights.
    float shiniess{};
    // Ni: index of refraction.
    float ior{};

    // map_Ka: ambient texture map.
    // map_Kd: diffuse texture map.
    // map_Ks: specular texture map.
    std::unique_ptr<Texture> diffuse_map{};

    // map_D/map_Tr: alpha texture map.
    // map_Bump: bump texture map.
    // map_Disp: displacement map.

    // --------------------------------------
    // Additional property for pbr model.
    // See https://github.com/tinyobjloader/tinyobjloader/blob/release/pbr-mtl.md.
    // --------------------------------------

    // Ke: emissive.
    glm::vec3 emissive{};
    // Pr: roughness.
    float roughness;
    // Pm: metallic.
    float metallic;
    // Ps: sheen.
    // Pc: clearcoat thickness.
    // Pcr: clearcoat roughness.
    // Aniso: anisotropy.
    // Anisor: anisotropy rotation.

    // map_Pr: roughness map.
    std::unique_ptr<Texture> roughness_map{};
    // map_Pm: metallic map.
    std::unique_ptr<Texture> metallic_map{};
    // map_Ke: emissive map.
    std::unique_ptr<Texture> emissive_map{};
    // map_Ps: sheen map.

    // Norm: normal map.
    std::unique_ptr<Texture> normal_map{};
};
