#pragma once

#include "scene/mesh.hpp"
#include "scene/material.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <vector>
#include <memory>

class Model final
{
public:
    Model() = default;
    Model(std::string const &path);

    static std::unique_ptr<Model> construct_skybox(const std::string &skybox_path, const std::string &skybox_name);
    static std::unique_ptr<Model> construct_equirectangular(const std::string &rect_path);
    static std::unique_ptr<Model> construct_cubemap(std::vector<std::string> cubemap_path);
    static std::unique_ptr<Model> construct_cube();
    static std::unique_ptr<Model> construct_quad();

    // Model data storage.
    vector<std::unique_ptr<Mesh>> meshes;
    vector<std::unique_ptr<Material>> materials;

    std::string model_name{};
    glm::vec3 position{0.0, 0.0, 0.0};
    glm::vec3 rotation{0.0, 0.0, 0.0};
    float scaling{1.0f};

private:
    // Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void load_model(std::string const &path);

    // Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void process_node(aiNode *node, const aiScene *scene);
    std::unique_ptr<Mesh> process_mesh(aiMesh *mesh, const aiScene *scene);
    std::unique_ptr<Material> process_material(aiMaterial *material, const aiScene *scene);

    // Helper function for material loading.
    std::unique_ptr<Texture> gen_material_texture(aiMaterial *mat, aiTextureType type, const std::string &type_name);

    std::string directory;
};
