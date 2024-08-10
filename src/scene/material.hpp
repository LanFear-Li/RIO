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
    glm::vec3 ambient{0.0f};
    // Kd: diffuse reflectivity.
    glm::vec3 diffuse{1.0f};
    // Ks: specular reflectivity.
    glm::vec3 specular{0.0f};
    // Tr: transparency.
    // Some implementations use 'd' as 'dissolve' and Tr = 1 - d.
    // Tf: transmission filter color (additionally with Tr).
    glm::vec3 transparency{0.0f};
    // Ns: shiniess or specular highlights.
    float shiniess{0.0f};
    // Ni: index of refraction.
    float ior{0.0f};

    // map_Ka: ambient texture map.
    // map_Kd: diffuse texture map.
    std::unique_ptr<Texture> diffuse_map{};
    // map_Ks: specular texture map.
    std::unique_ptr<Texture> specular_map{};

    // map_D/map_Tr: alpha texture map.
    // map_Bump: bump texture map.
    // map_Disp: displacement map.

    // --------------------------------------
    // Additional property for pbr model.
    // See https://github.com/tinyobjloader/tinyobjloader/blob/release/pbr-mtl.md.
    // --------------------------------------

    // Ke: emissive.
    glm::vec3 emissive{0.0f};
    // Pr: roughness.
    float roughness{1.0f};
    // Pm: metallic.
    float metallic{0.0f};
    // Ps: sheen.
    // Pc: clearcoat thickness.
    // Pcr: clearcoat roughness.
    // Aniso: anisotropy.
    // Anisor: anisotropy rotation.

    // map_Ke: emissive map.
    std::unique_ptr<Texture> emissive_map{};
    // map_Pr: roughness map.
    std::unique_ptr<Texture> roughness_map{};
    // map_Pm: metallic map.
    std::unique_ptr<Texture> metallic_map{};
    // map_Ps: sheen map.

    // Norm: normal map.
    std::unique_ptr<Texture> normal_map{};
};
