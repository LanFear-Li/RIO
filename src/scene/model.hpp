#pragma once

#include "scene/mesh.hpp"
#include "scene/material.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <memory>

class Model
{
public:
    // Model data storage.
    vector<std::unique_ptr<Mesh>> meshes;
    vector<std::unique_ptr<Material>> materials;
    std::string directory;
    bool gammaCorrection;

    // constructor, expects a filepath to a 3D model.
    Model() = default;
    Model(std::string const &path, bool gamma = false);

    static std::unique_ptr<Model> constructIBL(std::vector<std::string> ibl_path);

private:
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(std::string const &path);

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode *node, const aiScene *scene);
    std::unique_ptr<Mesh> processMesh(aiMesh *mesh, const aiScene *scene);
    std::unique_ptr<Material> processMaterial(aiMaterial *material, const aiScene *scene);

    // Helper function for material loading.
    std::unique_ptr<Texture> genMaterialTexture(aiMaterial *mat, aiTextureType type, std::string typeName);
};
