#pragma once

#include "scene/texture.hpp"

#include <glm/glm.hpp>

#include <iostream>
#include <memory>

struct IBL_Data
{
    std::shared_ptr<Texture> environment_map;
    std::unique_ptr<Texture> irrandiance_map;
    std::unique_ptr<Texture> prefiltered_map;
    std::unique_ptr<Texture> precomputed_brdf;
};

struct Material
{
public:
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
    glm::vec3 transparency{0.0f};
    // Tf: transmission filter color (additionally with Tr).
    // Ns: shiniess or specular highlights.
    float shiniess{0.0f};
    // Ni: index of refraction.
    float ior{0.0f};

    // map_Ka: ambient texture map.
    std::unique_ptr<Texture> ambient_map{};
    // map_Kd: diffuse texture map.
    // Used as albedo map in gltf model.
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
    // Combine roughness(G) and metallic(B) in gltf model.
    std::unique_ptr<Texture> metal_roughness_map{};
    // map_Ps: sheen map.

    // Ambient occlusion.
    std::unique_ptr<Texture> ao_map{};

    // Norm: normal map.
    std::unique_ptr<Texture> normal_map{};
    std::shared_ptr<Texture> ibl_map{};
};
